// project.cpp

#include "project.h"

#include <cassert>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Multitrack {

Project::Project() {
}

const QString &Project::name() const {
  return name_;
}

const Project::SourceType &Project::source_type() const {
  return source_type_;
}

const QString &Project::video_path() const {
  return video_path_;
}

int Project::first_frame() const {
  return first_frame_;
}

int Project::last_frame() const {
  return last_frame_;
}

const QStringList &Project::frame_paths() const {
  return frame_paths_;
}

float Project::fps() const {
  return fps_;
}

const QVector<TrackingTask *> &Project::tasks() const {
  return tasks_;
}

void Project::set_name(const QString &name) {
  name_ = name;
}

void Project::set_source_type(const SourceType &source_type) {
  source_type_ = source_type;
}

void Project::set_video_path(const QString &video_path) {
  video_path_ = video_path;
}

void Project::set_first_frame(int first_frame) {
  first_frame_ = first_frame;
}

void Project::set_last_frame(int last_frame) {
  last_frame_ = last_frame;
}

void Project::set_frame_paths(const QStringList &frame_paths) {
  frame_paths_ = frame_paths;
}

void Project::set_fps(float fps) {
  fps_ = fps;
}

bool Project::IsValid(QString *error) const {
  assert(error != nullptr);

  if (name_.isEmpty()) {
    *error = "Name cannot be empty.";
    return false;
  }

  if (source_type_ == kSourceTypeVideo) {
    cv::VideoCapture cap(video_path_.toStdString());
    if (!cap.isOpened()) {
      *error = "Video " + video_path_ + " cannot be found or read.";
      return false;
    }

    int nb_frames = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_COUNT));
    if (nb_frames == 0) {
      *error = "Video is empty.";
      return false;
    }
    if (first_frame_ < 1 || first_frame_ > nb_frames) {
      *error = "First frame should be between 1 and " +
               QString::number(nb_frames) + ".";
      return false;
    }
    if (last_frame_ < first_frame_ || last_frame_ > nb_frames) {
      *error = "Last frame should be between " +
               QString::number(first_frame_) + " and " +
               QString::number(nb_frames) + ".";
      return false;
    }
  } else {
    if (frame_paths_.count() == 0) {
      *error = "No frames were provided.";
      return false;
    }
    if (fps_ < 1.0f || fps_ >= 100.0f) {
      *error = "FPS should be between 1 and 100.";
      return false;
    }
  }

  return true;
}

QDataStream &operator<<(QDataStream &out, const Project &p) {
  int c = (p.source_type_ == Project::kSourceTypeVideo) ? 1 : 2;
  out << p.name_ << quint32(c) << p.video_path_ <<
         quint32(p.first_frame_) << quint32(p.last_frame_) << p.frame_paths_ <<
         p.fps_;

  out << p.tasks_.count();
  for (const TrackingTask *task : p.tasks_)
    out << task;

  return out;
}

QDataStream &operator>>(QDataStream &in, Project &p) {
  int c = 0;
  in >> p.name_ >> c >> p.video_path_ >> p.first_frame_ >>
        p.last_frame_ >> p.frame_paths_ >> p.fps_;

  int n = 0;
  in >> n;
  p.tasks_.resize(n);
  for (int i = 0; i < n; ++i) {
    p.tasks_[i] = new TrackingTask;
    in >> p.tasks_[i];
  }

  p.source_type_ = (c == 1) ? Project::kSourceTypeVideo :
                              Project::kSourceTypeFrames;

  return in;
}

void Project::add_tracking_task(TrackingTask *task) {
  tasks_.push_back(task);
}

void Project::remove_task(int index) {
  tasks_.remove(index);
}

void Project::clear_tasks() {
  tasks_.clear();
}

}  // namespace Multitrack
