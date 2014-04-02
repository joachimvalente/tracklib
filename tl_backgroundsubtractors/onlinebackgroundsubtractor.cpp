#include "tl_backgroundsubtractors/onlinebackgroundsubtractor.h"

namespace tl {

OnlineBackgroundSubtractor::OnlineBackgroundSubtractor(
    const cv::Mat &initial_frame,
    BackgroundSubtractionMethod method) :
  BackgroundSubtractor(initial_frame) {
  switch(method) {
    case TL_GMG:
      opencv_bgs_ = new cv::BackgroundSubtractorGMG;
      break;
    case TL_MOG:
      opencv_bgs_ = new cv::BackgroundSubtractorMOG;
      break;
    case TL_MOG2:
      opencv_bgs_ = new cv::BackgroundSubtractorMOG2;
      break;
  }
}

OnlineBackgroundSubtractor::~OnlineBackgroundSubtractor() {
  delete opencv_bgs_;
}

void OnlineBackgroundSubtractor::Compute() {
  (*opencv_bgs_)(frame_, background_);
}

}  // namespace tl
