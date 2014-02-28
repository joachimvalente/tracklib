#include "tl_detectors/dummydetector.h"

using namespace cv;

namespace tl {

//------------------------ Constructor ------------------------
DummyDetector::DummyDetector(const cv::Mat &initial_frame,
                             cv::Rect initial_state) :
  Detector(initial_frame, initial_state) {}

//----------------------- Main functions -----------------------
/*!
 * \brief Leave the state and confidence unchanged.
 */
void DummyDetector::Detect() {}

std::string DummyDetector::ToString() const {
  return "dummy detector";
}

}  // namespace tl
