#include "tracker.h"

#include "../common.h"
#include "../tl_util/opencv.h"

using namespace cv;

namespace tl {

Tracker::Tracker(const cv::Mat &initial_frame, cv::Rect initial_state) :
  initial_frame_(initial_frame),
  initial_state_(initial_state) {
  CHECK(IsRectInsideFrame(initial_frame, initial_state));
}

void Tracker::NextFrame(const Mat &frame) {
  frame_ = frame;
}

Rect Tracker::state() const {
  return state_;
}

float Tracker::confidence() const {
  return confidence_;
}

void Tracker::set_state(Rect state, float confidence) {
  CHECK(0.0f <= confidence && confidence <= 1.0f);
  CHECK(IsRectInsideFrame(frame_, state));
  state_ = state;
  confidence_ = confidence;
}

const Mat &Tracker::frame() const {
  return frame_;
}

}  // namespace tl
