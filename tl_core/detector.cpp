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
  confidence_(1.0f),
  width_(initial_frame.cols),
  height_(initial_frame.rows),
  channels_(initial_frame.channels()) {
  CHECK_NOTNULL(initial_frame.data);
  CHECK(FrameCanvas(initial_frame).area() > 0);
  CHECK(IsRectInsideFrame(initial_frame, initial_state));
  CHECK(initial_state.area() > 0);
  CHECK_MSG(channels_ == 1 || channels_ == 3,
            "images have to be color or grayscale");
}

void Detector::NextFrame(const Mat &frame) {
  // Check consistency between frames.
  CHECK_NOTNULL(frame.data);
  CHECK(frame.cols == width_);
  CHECK(frame.rows == height_);
  CHECK(frame.channels() == channels_);

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

const int Detector::width() const {
  return width_;
}

const int Detector::height() const {
  return height_;
}

const int Detector::channels() const {
  return channels_;
}

}  // namespace tl
