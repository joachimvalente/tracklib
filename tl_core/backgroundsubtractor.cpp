#include "tl_core/backgroundsubtractor.h"

#include <iostream>

using namespace cv;

namespace tl {

BackgroundSubtractor::BackgroundSubtractor(const cv::Mat &initial_frame) :
  frame_(initial_frame) {
  background_ = cv::Mat::zeros(initial_frame.rows, initial_frame.cols, CV_8U);
}

void BackgroundSubtractor::NextFrame(const Mat &frame) {
  frame_ = frame.clone();
  Compute();
}

const cv::Mat &BackgroundSubtractor::background() const {
  return background_;
}

cv::Mat BackgroundSubtractor::foreground() const {
  return 1 - background_;
}

cv::Mat BackgroundSubtractor::GetForeground() const {
  Mat fg = Mat::zeros(frame_.size(), frame_.type());
  frame_.copyTo(fg, background());
  return fg;
}

}  // namespace tl
