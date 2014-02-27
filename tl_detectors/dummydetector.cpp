#include "dummydetector.h"

using namespace cv;

namespace tl {

DummyDetector::DummyDetector(const cv::Mat &initial_frame,
                             cv::Rect initial_state) :
  Detector(initial_frame, initial_state) {}

/*!
 * \brief Leave the state and confidence unchanged.
 */
void DummyDetector::Detect() {}

}  // namespace tl
