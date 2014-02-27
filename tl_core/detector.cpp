#include "detector.h"

#include "../common.h"
#include "../tl_util/opencv.h"

using namespace cv;

namespace tl {

Detector::Detector(const cv::Mat &initial_frame, cv::Rect initial_state) :
  initial_frame_(initial_frame),
  initial_state_(initial_state),
  frame_(initial_frame),
  state_(initial_state),
  confidence_(1.0f) {
  CHECK(IsRectInsideFrame(initial_frame, initial_state));
  CHECK(initial_state.area() > 0);
}

void Detector::NextFrame(const Mat &frame) {
  frame_ = frame;
}

Rect Detector::state() const {
  return state_;
}

float Detector::confidence() const {
  return confidence_;
}

void Detector::set_state(Rect state, float confidence) {
  CHECK(0.0f <= confidence && confidence <= 1.0f);
  CHECK(IsRectInsideFrame(frame_, state));
  CHECK(state.area() > 0);
  state_ = state;
  confidence_ = confidence;
}

const Mat &Detector::frame() const {
  return frame_;
}

}  // namespace tl
