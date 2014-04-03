// trackingtask.cpp

#include "trackingtask.h"

#include <cassert>

#include <QTime>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "tracklib.h"

using namespace tl;

namespace Multitrack {

TrackingTask::TrackingTask() :
  filter_(kNoFilter), bgs_(kNoBgs),
  completed_(false), active_(false) {
  set_random_color();
}

TrackingTask::TrackingTask(const TrackingTask *task) {
  algo_ = task->algo_;
  params_ = task->params_;
  filter_ = task->filter_;
  filter_params_ = task->filter_params_;
  bgs_ = task->bgs_;
  bgs_params_ = task->bgs_params_;
  object_ = task->object_;
  first_frame_ = task->first_frame_;
  set_random_color();
  ClearResults();
}

void TrackingTask::set_tracker(Algorithm algo,
                               const QVector<Param> &params) {
  algo_ = algo;
  params_ = params;
}

void TrackingTask::set_filter(Filter filter,
                              const QVector<Param> &filter_params) {
  filter_ = filter;
  filter_params_ = filter_params;
}

void TrackingTask::set_bgs(Bgs bgs, const QVector<Param> &bgs_params) {
  bgs_ = bgs;
  bgs_params_ = bgs_params;
}

TrackingTask::Algorithm TrackingTask::algorithm() const {
  return algo_;
}

TrackingTask::Filter TrackingTask::filter() const {
  return filter_;
}

TrackingTask::Bgs TrackingTask::bgs() const {
  return bgs_;
}

QString TrackingTask::algorithm_str() const {
  QString alg;

  if (algo_ == kTemplateMatching)
    alg = "Template matching";
  else if (algo_ == kMeanshift)
    alg = "Meanshift";

  if (filter_ == kKalmanFilter)
    alg += "/Kalman";

  if (bgs_ != kNoBgs)
    alg += " + BGS";

  return alg;
}

const Param &TrackingTask::param(int i) const {
  assert(0 <= i && i < params_.count());
  return params_.at(i);
}

const Param &TrackingTask::filter_param(int i) const {
  assert(0 <= i && i < filter_params_.count());
  return filter_params_.at(i);
}

const Param &TrackingTask::bgs_param(int i) const {
  assert(0 <= i && i < bgs_params_.count());
  return bgs_params_.at(i);
}

void TrackingTask::set_object(const cv::Rect &object) {
  object_ = CvRect2QRect(object);
}

void TrackingTask::set_first_frame(int first_frame) {
  first_frame_ = first_frame;
}

void TrackingTask::set_project_details(bool is_video,
                                       const QString &video_path,
                                       const QStringList &frame_paths) {
  is_video_ = is_video;
  video_path_ = QString(video_path);
  frame_paths_ = QStringList(frame_paths);
}

void TrackingTask::set_color(const QColor &color) {
  color_ = QColor(color);
}

void TrackingTask::set_random_color() {
  set_color(QColor(rand_int(0, 255),
                   rand_int(0, 255),
                   rand_int(0, 255)));
}

void TrackingTask::set_active(bool active) {
  if (completed_) active_ = active;
}

QRect TrackingTask::result(int frame) const {
  if (frame < first_frame_) return QRect();
  if (results_.count() < frame - first_frame_ + 1) return QRect();

  return results_.at(frame - first_frame_);
}

const QVector<QRect> &TrackingTask::results() const {
  return results_;
}

bool TrackingTask::completed() const {
  return completed_;
}

bool TrackingTask::active() const {
  return active_;
}

int TrackingTask::first_frame() const {
  return first_frame_;
}

const QColor &TrackingTask::color() const {
  return color_;
}

const QRect &TrackingTask::object() const {
  return object_;
}

const QString &TrackingTask::error() const {
  return error_;
}

const cv::Mat &TrackingTask::preview() const {
  return preview_;
}

void TrackingTask::ClearResults() {
  completed_ = false;
  active_ = false;
  results_.clear();
}

QDataStream &operator<<(QDataStream &out, const TrackingTask *t) {
  out << quint32(static_cast<int>(t->algo_)) << t->params_ <<
         quint32(static_cast<int>(t->filter_)) << t->filter_params_ <<
         quint32(static_cast<int>(t->bgs_)) << t->bgs_params_ <<
         t->object_ << t->first_frame_ << t->completed_ << t->results_ <<
         t->active_ << t->color_;
  return out;
}

QDataStream &operator>>(QDataStream &in, TrackingTask *t) {
  assert(t != nullptr);
  int c = 0;
  int d = 0;
  int e = 0;
  in >> c >> t->params_ >> d >> t->filter_params_ >> e >> t->bgs_params_ >>
        t->object_ >> t->first_frame_ >> t->completed_ >> t->results_ >>
        t->active_ >> t->color_;
  t->algo_ = static_cast<TrackingTask::Algorithm>(c);
  t->filter_ = static_cast<TrackingTask::Filter>(d);
  t->bgs_ = static_cast<TrackingTask::Bgs>(e);
  return in;
}

void TrackingTask::Run() {
  completed_ = false;
  active_ = false;

  cv::VideoCapture cap;
  cv::Mat frame;
  if (is_video_) {
    cap.open(video_path_.toStdString());
    if (!cap.isOpened()) {
      error_ = "Could not open video.";
      emit Failed();
      return;
    }
    cap >> frame;
    if (!frame.data) {
      error_ = "Video is empty.";
      emit Failed();
      return;
    }
  } else {
    if (frame_paths_.count() == 0) {
      error_ = "No frames were provided.";
      emit Failed();
      return;
    }
    frame = cv::imread(frame_paths_.at(0).toStdString());
  }

  QTime timer;
  timer.start();

  int index = 0;
  while (index + 1 < first_frame_) {
    if (is_video_) {
      ++index;
      cap >> frame;
      if (!frame.data) {
        error_ = "Not enough frames";
        emit Failed();
        return;
      }
    } else {
      ++index;
      if (index >= frame_paths_.count()) {
        error_ = "Not enough frames";
        emit Failed();
        return;
      }
    }
  }

  Detector *detector = nullptr;
  switch (algo_) {
    case TrackingTask::kTemplateMatching:
    {
      const int methods[] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED,
                             CV_TM_CCORR, CV_TM_CCORR_NORMED,
                             CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED};
      TemplateMatchingDetector *m_detector = new TemplateMatchingDetector(
                                              frame, QRect2CvRect(object_));
      m_detector->set_opencv_method(methods[params_.at(0).GetI()]);
      detector = m_detector;
      break;
    }
    case TrackingTask::kMeanshift:
    {
      const int channels[] = {TL_H, TL_S, TL_HS, TL_GRAY};
      MeanshiftDetector *m_detector = new MeanshiftDetector(
                                       frame, QRect2CvRect(object_));
      m_detector->set_variant(
            static_cast<MeanshiftVariant>(params_.at(0).GetI()));
      m_detector->set_channels_to_use(
            static_cast<Channels>(channels[params_.at(1).GetI()]));
      m_detector->set_max_iter(params_.at(2).GetI());
      detector = m_detector;
      break;
    }
    default:
    {
      detector = new NoDetector(frame, QRect2CvRect(object_));
      break;
    }
  }
  Tracker tracker;
  tracker.set_detector(detector);

  switch (filter_) {
    case kKalmanFilter:
    {
      tl::KalmanFilter *filter = new tl::KalmanFilter(
                                   QRect2CvRect(object_),
                                   filter_params_.at(0).GetF(),
                                   filter_params_.at(1).GetF());
      tracker.set_filter(filter);
      break;
    }
    case kNoFilter:
    default:
    {
      // Do nothing.
    }
  }

  switch (bgs_) {
    case kOnlineBgs:
    {
      OnlineBackgroundSubtractor *bgs =
          new OnlineBackgroundSubtractor(
            frame,
            static_cast<BackgroundSubtractionMethod>(bgs_params_.at(0).GetI()));
      tracker.set_bgs(bgs);
      break;
    }
    case kNoBgs:
    default:
    {
      // Do nothing.
    }
  }

  results_.clear();
  results_.push_back(object_);
  while (true) {
    if (is_video_) {
      ++index;
      cap >> frame;
      if (!frame.data)
        break;
    } else {
      ++index;
      if (index >= frame_paths_.count())
        break;
      frame = cv::imread(frame_paths_.at(index).toStdString());
    }

    tracker.Track(frame);
    cv::Rect object = tracker.state();

    results_.push_back(CvRect2QRect(object));
    emit Processed(index - first_frame_ + 1);
    if (timer.elapsed() > 100) {
      cv::Mat overlay(frame.rows, frame.cols, frame.type(),
                      cv::Scalar::all(0));
      cv::rectangle(overlay, object, cv::Scalar(0, 255, 255), CV_FILLED);
      cv::addWeighted(frame, 1.0, overlay, 0.5, 0, preview_);
      emit Preview();
      timer.restart();
    }
  }

  emit Processed(index - first_frame_ + 1);
  completed_ = true;
  active_ = true;
  emit Finished();
}

QRect TrackingTask::CvRect2QRect(const cv::Rect &r) {
  QRect q;
  q.setX(r.x);
  q.setY(r.y);
  q.setWidth(r.width);
  q.setHeight(r.height);
  return q;
}

cv::Rect TrackingTask::QRect2CvRect(const QRect &q) {
  cv::Rect r;
  r.x = q.x();
  r.y = q.y();
  r.width = q.width();
  r.height = q.height();
  return r;
}

int TrackingTask::rand_int(int low, int high) {
  return qrand() % ((high + 1) - low) + low;
}

}  // namespace Multitrack
