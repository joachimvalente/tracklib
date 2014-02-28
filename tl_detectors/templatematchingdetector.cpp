#include "tl_detectors/templatematchingdetector.h"

namespace tl {

//------------------------- Constructor -------------------------
TemplateMatchingDetector::TemplateMatchingDetector(const Mat &initial_frame,
                                                   Rect initial_state) :
  Detector(initial_frame, initial_state),
  template_(initial_frame(initial_state).clone()),
  opencv_method_(CV_TM_SQDIFF) {}

//------------------------- Main methods ------------------------
void TemplateMatchingDetector::Detect() {
  Mat result;
  matchTemplate(get_frame(), template_, result, opencv_method_);

  Point location;
  if (opencv_method_ == CV_TM_SQDIFF || opencv_method_ == CV_TM_SQDIFF_NORMED) {
    minMaxLoc(result, nullptr, nullptr, &location, nullptr);  // Locate min.
  } else {
    minMaxLoc(result, nullptr, nullptr, nullptr, &location);  // Locate max.
  }
  set_state(location);
}

std::string TemplateMatchingDetector::ToString() const {
  return "template matching detector";
}

//------------------------ Public accessors -------------------------
void TemplateMatchingDetector::set_opencv_method(int opencv_method) {
  CHECK_MSG(opencv_method == CV_TM_CCOEFF ||
            opencv_method == CV_TM_CCOEFF_NORMED ||
            opencv_method == CV_TM_CCORR ||
            opencv_method == CV_TM_CCORR_NORMED ||
            opencv_method == CV_TM_SQDIFF ||
            opencv_method == CV_TM_SQDIFF_NORMED,
            "invalid method");
  opencv_method_ = opencv_method;
}

}  // namespace tl
