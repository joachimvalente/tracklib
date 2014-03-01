#include "tl_core/detector.h"

#include "tl_util/geometry.h"

using namespace cv;
using namespace tl::internal;

namespace tl {

//------------------------- Constructor ---------------------------
Detector::Detector(const cv::Mat &initial_frame, cv::Rect initial_state) :
  width_(initial_frame.cols),
  height_(initial_frame.rows),
  channels_(initial_frame.channels()),
  depth_(initial_frame.depth()),
  initial_frame_(initial_frame.clone()),
  initial_state_(initial_state),
  frame_(initial_frame_.clone()),
  state_(initial_state),
  confidence_(1.0f) {
  // Safety checks.
  CHECK_NOTNULL(initial_frame.data);
  CHECK(initial_state.area() > 0);
  CHECK(IsRectInsideFrame(initial_state, initial_frame));
  CHECK_MSG(channels_ == 1 || channels_ == 3,
            "images have to be color or grayscale");
  CHECK_MSG(depth_ == CV_8U || depth_ == CV_32F,
            "supported depths are CV_8U and CV_32F");
}

//------------------------------ Main methods -------------------------
void Detector::NextFrame(const Mat &frame) {
  // Check consistency between frames.
  CHECK_NOTNULL(frame.data);
  CHECK(frame.cols == width_);
  CHECK(frame.rows == height_);
  CHECK(frame.channels() == channels_);
  CHECK(frame.depth() == depth_);

  frame_ = frame.clone();
}

std::string Detector::ToString() const {
  return "undocumented detector";
}

//--------------------------- Public accessors -----------------------
cv::Rect Detector::state() const {
  return state_;
}

void Detector::set_state(cv::Rect state, float confidence) {
  CHECK(0.0f <= confidence && confidence <= 1.0f);
  if (!IsRectInsideFrame(state, frame_)) {
    WARNING("given state " << state << " is outside the frame");
  } else if (state.area() == 0) {
    WARNING("given state is empty");
  }

  state_ = state;
  confidence_ = confidence;
}

void Detector::set_state(cv::Point tl, float confidence) {
  Rect state = state_;
  state.x = tl.x;
  state.y = tl.y;
  set_state(state, confidence);
}

float Detector::confidence() const {
  return confidence_;
}

void Detector::set_confidence(float confidence) {
  CHECK(0.0f <= confidence && confidence <= 1.0f);
  confidence_ = confidence;
}

//------------------------- Protected accessors ---------------------
const cv::Mat &Detector::initial_frame() const {
  return initial_frame_;
}

Mat Detector::initial_frame(Rect roi) const {
  return initial_frame_(roi);
}

const cv::Rect &Detector::initial_state() const {
  return initial_state_;
}

const cv::Mat &Detector::frame() const {
  return frame_;
}

int Detector::width() const {
  return width_;
}

int Detector::height() const {
  return height_;
}

int Detector::channels() const {
  return channels_;
}

int Detector::depth() const {
  return depth_;
}

}  // namespace tl
