#include "tl_detectors/nodetector.h"

using namespace cv;

namespace tl {

//------------------------ Constructor ------------------------
NoDetector::NoDetector(const cv::Mat &initial_frame, cv::Rect initial_state) :
  Detector(initial_frame, initial_state) {}

//----------------------- Main functions -----------------------
/*!
 * \brief Leave the state and confidence unchanged.
 */
void NoDetector::Detect() {}

std::string NoDetector::ToString() const {
  return "no detector";
}

}  // namespace tl
