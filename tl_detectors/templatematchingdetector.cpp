#include "templatematchingdetector.h"

#include "../common.h"

namespace tl {

TemplateMatchingDetector::TemplateMatchingDetector(
    const cv::Mat &initial_frame,
    cv::Rect initial_state,
    TemplateMatchingVariant variant,
    TemplateMatchingFlags flags) :
  Detector(initial_frame, initial_state),
  variant_(variant),
  flags_(flags) {
  CHECK_MSG((flags & 0x000F) == kTemplateMatchingGrayscale || channels() == 3,
            "must use grayscale mode for grayscale images");
}

void TemplateMatchingDetector::Detect() {

}

}  // namespace tl
