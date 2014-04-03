// videoplayer.cpp

#include "videoplayer.h"

#include <cmath>

namespace Multitrack {

VideoPlayer::VideoPlayer(const QString &filename, int first_frame,
                         int last_frame) :
  AbstractPlayer(),
  filename_(filename), first_frame_(first_frame), last_frame_(last_frame) {
  nb_frames_ = last_frame_ - first_frame_ + 1;
  timer_.start();
}

void VideoPlayer::Initialize() {
  cap_.open(filename_.toStdString());
  if (!cap_.isOpened()) {
    emit CriticalError("Could not open video.");
    return;
  }

  cap_.set(CV_CAP_PROP_POS_FRAMES, first_frame_ - 1);
  cv::Mat frame;
  cap_ >> frame;
  if (!frame.data) {
    emit CriticalError("Video is empty.");
    return;
  }

  fps_ = static_cast<float>(cap_.get(CV_CAP_PROP_FPS));

  // Transform the frame for display.
  ProcessFrame(frame, QTime::currentTime());
}

void VideoPlayer::Previous() {
  current_frame_ = std::fmax(0, current_frame_ - 2);
  cap_.set(CV_CAP_PROP_POS_FRAMES, first_frame_ + current_frame_ - 1);
  Next();
}

void VideoPlayer::First() {
  GotoFrame(0);
}

void VideoPlayer::Next() {
  cv::Mat frame;
  cap_ >> frame;
  if (!frame.data || nb_frames_ <= current_frame_) {
    Pause();
  } else {
    ++current_frame_;
    ProcessFrame(frame, QTime::currentTime());
  }
}

void VideoPlayer::GotoFrame(int frame) {
  // Ensure we don't fetch too many images at once (e.g. with the slider).
  if (timer_.elapsed() > 100) {
    timer_.restart();
    current_frame_ = std::fmax(0, std::fmin(nb_frames_, frame - 1));
    cap_.set(CV_CAP_PROP_POS_FRAMES, first_frame_ + current_frame_ - 1);
    Next();
  }
}

}  // namespace Multitrack
