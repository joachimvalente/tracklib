#include "detector.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../tl_util/opencv.h"

using namespace cv;
using namespace std;

namespace tl {

Detector::Detector(const cv::Mat &initial_frame, cv::Rect initial_state,
                   DetectorFlags g_flags) :
  width_(initial_frame.cols),
  height_(initial_frame.rows),
  channels_(initial_frame.channels()),
  g_flags_(g_flags),
  initial_frame_(ConvertColorAndType(initial_frame)),
  initial_state_(initial_state),
  frame_(initial_frame_.clone()),
  state_(initial_state),
  confidence_(1.0f) {
  CHECK_NOTNULL(initial_frame.data);
  CHECK(FrameCanvas(initial_frame).area() > 0);
  CHECK(IsRectInsideFrame(initial_frame, initial_state));
  CHECK(initial_state.area() > 0);
  CHECK_MSG(channels_ == 1 || channels_ == 3,
            "images have to be color or grayscale");
  CheckFlags();
}

void Detector::NextFrame(const Mat &frame) {
  // Check consistency between frames.
  CHECK_NOTNULL(frame.data);
  CHECK(frame.cols == width_);
  CHECK(frame.rows == height_);
  CHECK(frame.channels() == channels_);

  ConvertColorAndType(frame, frame_);
}

cv::Rect Detector::state() const {
  return state_;
}

float Detector::confidence() const {
  return confidence_;
}

void Detector::set_state(Rect state, float confidence) {
  CHECK(0.0f <= confidence && confidence <= 1.0f);
  if (!IsRectInsideFrame(frame_, state)) {
    confidence_ = 0.0f;
    WARNING("given state is outside the frame - ignoring");
  } else if (state.area() == 0) {
    confidence_ = 0.0f;
    WARNING("given state is empty - ignoring");
  } else {
    state_ = state;
    confidence_ = confidence;
  }
}

void Detector::set_state(Point tl, float confidence) {
  Rect state = state_;
  state.x = tl.x;
  state.y = tl.y;
  set_state(state, confidence);
}

std::string Detector::ToString() const {
  return "undocumented detector";
}

const cv::Mat &Detector::initial_frame() const {
  return initial_frame_;
}

const cv::Rect &Detector::initial_state() const {
  return initial_state_;
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

cv::Mat Detector::ConvertColorAndType(const Mat &in) {
  Mat out;
  ConvertColorAndType(in, out);
  return out;
}

void Detector::ConvertColorAndType(const Mat &in, Mat& out) {
  switch (g_flags_ & 0x000f) {
    case kDetectorGrayscale:
      if (channels_ == 3) {
        cvtColor(in, out, CV_RGB2GRAY);
      }
      break;
    case kDetectorHue:
    {
      cvtColor(in, out, CV_RGB2HSV);
      vector<Mat> channels;
      split(out, channels);
      merge(&channels[0], 1, out);
      break;
    }
    case kDetectorSaturation:
    {
      cvtColor(in, out, CV_RGB2HSV);
      vector<Mat> channels;
      split(out, channels);
      merge(&channels[1], 1, out);
      multiply(out, 255, out);  // Saturation between 0 and 1.
      break;
    }
    case kDetectorHs:
    {
      cvtColor(in, out, CV_RGB2HSV);
      vector<Mat> channels;
      split(out, channels);
      multiply(channels[1], 255, channels[1]);  // Saturation between 0 and 1.
      merge(&channels[0], 2, out);
      break;
    }
    case kDetectorRgb:
      out = in.clone();
      break;
    default:
      DIE;
  }

  if ((g_flags_ & 0x00f0) == kDetector32F) {
    out.convertTo(out, (channels_ == 1) ? CV_32F : CV_32FC3);
  }
}

void Detector::CheckFlags() const {
  int color_mode = g_flags_ & 0x000f;
  CHECK_MSG(color_mode == kDetectorGrayscale ||
            color_mode == kDetectorHue ||
            color_mode == kDetectorSaturation ||
            color_mode == kDetectorHs ||
            color_mode == kDetectorRgb,
            "invalid flags");
  CHECK_MSG((g_flags_ & 0x000f) == kDetectorGrayscale || channels_ == 3,
            "must use grayscale mode for grayscale images");

  int value_type = g_flags_ & 0x00f0;
  CHECK_MSG(value_type == kDetector32F ||
            value_type == kDetector8U,
            "invalid flags");
}

}  // namespace tl
