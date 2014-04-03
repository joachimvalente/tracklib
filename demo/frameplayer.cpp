// frameplayer.cpp

#include "frameplayer.h"

#include <cmath>

namespace Multitrack {

FramePlayer::FramePlayer(const QStringList &frame_paths, float fps) :
  frame_paths_(frame_paths) {
  fps_ = fps;
  nb_frames_ = frame_paths_.count();
  timer_.start();
}

void FramePlayer::Initialize() {
  // Transform the frame for display.
  cv::Mat frame = cv::imread(frame_paths_.at(0).toStdString());
  ProcessFrame(frame, QTime::currentTime());
}

void FramePlayer::Previous() {
  current_frame_ = std::fmax(0, current_frame_ - 2);
  Next();
}

void FramePlayer::First() {
  GotoFrame(0);
}

void FramePlayer::Next() {
  if (current_frame_ < nb_frames_) {
    ++current_frame_;
    cv::Mat frame = cv::imread(
                      frame_paths_.at(current_frame_ - 1).toStdString());
    ProcessFrame(frame, QTime::currentTime());
  } else {
    Pause();
  }
}

void FramePlayer::GotoFrame(int frame) {
  // Ensure we don't fetch too many images at once (e.g. with the slider).
  if (timer_.elapsed() > 100) {
    timer_.restart();
    current_frame_ = std::fmax(0, std::fmin(nb_frames_, frame - 1));
    Next();
  }
}

}  // namespace Multitrack
