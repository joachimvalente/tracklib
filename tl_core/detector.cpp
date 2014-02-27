#include "detector.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "../tl_util/opencv.h"

using namespace cv;

namespace tl {

Detector::Detector(const cv::Mat &initial_frame, cv::Rect initial_state,
                   DetectorFlags g_flags) :
  initial_state_(initial_state),
  state_(initial_state),
  confidence_(1.0f),
  width_(initial_frame.cols),
  height_(initial_frame.rows),
  channels_(initial_frame.channels()),
  g_flags_(g_flags) {
  CHECK_NOTNULL(initial_frame.data);
  CHECK(FrameCanvas(initial_frame).area() > 0);
  CHECK(IsRectInsideFrame(initial_frame, initial_state));
  CHECK(initial_state.area() > 0);
  CHECK_MSG(channels_ == 1 || channels_ == 3,
            "images have to be color or grayscale");
  CHECK_MSG((g_flags & 0x000f) == kDetectorGrayscale || channels_ == 3,
            "must use grayscale mode for grayscale images");
  initial_frame.convertTo(initial_frame_,
                          (channels_ == 1) ? CV_32F : CV_32FC3);
  ConvertColor(initial_frame_);
  initial_frame_.copyTo(frame_);
}

void Detector::NextFrame(const Mat &frame) {
  // Check consistency between frames.
  CHECK_NOTNULL(frame.data);
  CHECK(frame.cols == width_);
  CHECK(frame.rows == height_);
  CHECK(frame.channels() == channels_);

  frame.convertTo(frame_, (channels_ == 1) ? CV_32F : CV_32FC3);
  ConvertColor(frame_);
}

cv::Rect Detector::state() const {
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

std::string Detector::ToString() const {
  return "undocumented detector";
}

const cv::Mat &Detector::initial_frame() const {
  return initial_frame_;
}

const cv::Mat &Detector::frame() const {
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

DetectorFlags Detector::g_flags() const {
  return g_flags_;
}

void Detector::ConvertColor(Mat &frame) {
  switch (g_flags_ & 0x000f) {
    case kDetectorGrayscale:
      if (channels_ == 3) {
        cvtColor(frame, frame, CV_RGB2GRAY);
      }
      break;
    case kDetectorHue:
    case kDetectorSaturation:
    case kDetectorHs:
      cvtColor(frame, frame, CV_RGB2HSV);
      break;
    case kDetectorRgb:
      break;
    default:
      DIE;
  }
}

}  // namespace tl
