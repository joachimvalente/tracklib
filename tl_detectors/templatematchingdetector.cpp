#include "templatematchingdetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../tl_util/similaritymeasures.h"

using namespace cv;
using namespace std;

namespace tl {

TemplateMatchingDetector::TemplateMatchingDetector(
    const cv::Mat &initial_frame,
    cv::Rect initial_state,
    DetectorFlags g_flags,
    TemplateMatchingFlags flags) :
  Detector(initial_frame, initial_state, g_flags),
  flags_(flags),
  window_size_(20),
  opencv_method_(CV_TM_SQDIFF) {
  CheckFlags();
  INFO(ToString());

  // Save model.
  template_ = Detector::initial_frame()(initial_state);

  // Normalization of the template (if using NCC).
  if ((flags_ & 0x000f) == kTemplateMatchingNcc &&
      (flags_ & 0x00f0) == kTemplateMatchingBasic) {
    Normalize(template_, template_);
  }
}

void TemplateMatchingDetector::Detect() {
  switch (flags_ & 0x00f0) {
    case kTemplateMatchingBasic:
      DetectBasic();
      break;
    case kTemplateMatchingOpencv:
      DetectOpencv();
      break;
    default:
      DIE;
  }
}

void TemplateMatchingDetector::DetectBasic() {
  // Select distance function.
  SimilarityMeasure distance_function;
  switch (flags_ & 0x000f) {
    case kTemplateMatchingNcc:
      distance_function = NormalizedCrossCorrelationInv;
      break;
    case kTemplateMatchingMse:
      distance_function = MeanSquaredError;
      break;
    case kTemplateMatchingPsnr:
      distance_function = PeakSignalToNoiseRatio;
      break;
    case kTemplateMatchingDssim:
      distance_function = StructuralDissimilarity;
      break;
    case kTemplateMatchingSad:
      distance_function = SumOfAbsoluteDifferences;
      break;
    default:
      DIE;
  }

  // Sliding-window.
  Rect roi = state();
  float min_distance = -1.0f;
  Rect argmin = state();
  int min_x, max_x, min_y, max_y;
  if (window_size_ > 0) {
    min_x = std::max(0, state().x - window_size_);
    max_x = std::min(width() - template_.cols, state().x + window_size_);
    min_y = std::max(0, state().y - window_size_);
    max_y = std::min(height() - template_.rows, state().y + window_size_);
  } else {
    min_x = 0;
    max_x = width() - template_.cols;
    min_y = 0;
    max_y = height() - template_.rows;
  }
  for (int i = min_x; i < max_x; ++i) {
    for (int j = min_y; j < max_y; ++j) {
      roi.x = i;
      roi.y = j;
      Mat candidate = frame()(roi);
      float distance = distance_function(candidate, template_);

      if (min_distance < 0 || distance < min_distance) {
        min_distance = distance;
        argmin = roi;
      }
    }
  }

  // Update state.
  set_state(argmin);
}

void TemplateMatchingDetector::DetectOpencv() {
  Mat result(frame().cols - template_.cols + 1,
             frame().rows - template_.rows + 1,
             CV_32FC1);

  matchTemplate(frame(), template_, result, opencv_method_);
  Point location;
  if (opencv_method_ == CV_TM_SQDIFF || opencv_method_ == CV_TM_SQDIFF_NORMED) {
    minMaxLoc(result, nullptr, nullptr, &location, nullptr);  // Locate min.
  } else {
    minMaxLoc(result, nullptr, nullptr, nullptr, &location);  // Locate max.
  }
  set_state(location);
}

std::string TemplateMatchingDetector::ToString() const {
  string variant, color_mode, similarity_measure, additional_info;
  switch (flags_ & 0x00f0) {
    case kTemplateMatchingBasic:
      variant = "basic";
      additional_info = " windows_size=" + to_string(window_size_);
      break;
    case kTemplateMatchingOpencv:
      variant = "opencv";
      additional_info = " opencv_method=" + to_string(opencv_method_);
      break;
    default:
      DIE;
  }
  switch (g_flags() & 0x000f) {
    case kDetectorGrayscale:
      color_mode = "grayscale";
      break;
    case kDetectorHue:
      color_mode = "hue";
      break;
    case kDetectorSaturation:
      color_mode = "saturation";
      break;
    case kDetectorHs:
      color_mode = "hue+saturation";
      break;
    case kDetectorRgb:
      color_mode = "RGB";
      break;
    default:
      DIE;
  }
  switch (flags_ &0x000f) {
    case kTemplateMatchingNcc:
      similarity_measure = "NCC";
      break;
    case kTemplateMatchingMse:
      similarity_measure = "MSE";
      break;
    case kTemplateMatchingPsnr:
      similarity_measure = "PSNR";
      break;
    case kTemplateMatchingDssim:
      similarity_measure = "DSSIM";
      break;
    case kTemplateMatchingSad:
      similarity_measure = "SAD";
      break;
    default:
      DIE;
  }

  if ((flags_ & 0x00f0) == kTemplateMatchingBasic) {
    additional_info += " similarity_measure=" + similarity_measure;
  }

  return "template matching detector [variant=" + variant + " color_mode=" +
      color_mode + additional_info + "]";
}

int TemplateMatchingDetector::window_size() const {
  return window_size_;
}

void TemplateMatchingDetector::set_window_size(int window_size) {
  CHECK_MSG((flags_ & 0x00f0) == kTemplateMatchingBasic,
            "cannot set window size with this template matching variant");

  if (window_size >= 0) {
    window_size_ = window_size;
  } else {
    WARNING("window_size must be nonnegative - ignoring");
  }
}

void TemplateMatchingDetector::set_opencv_method(int opencv_method) {
  CHECK_MSG((flags_ & 0x00f0) == kTemplateMatchingOpencv,
            "not using OpenCV built-in function");
  CHECK_MSG(opencv_method == CV_TM_CCOEFF ||
            opencv_method == CV_TM_CCOEFF_NORMED ||
            opencv_method == CV_TM_CCORR ||
            opencv_method == CV_TM_CCORR_NORMED ||
            opencv_method == CV_TM_SQDIFF ||
            opencv_method == CV_TM_SQDIFF_NORMED,
            "invalid OpenCV comparison method");
  opencv_method_ = opencv_method;
}

void TemplateMatchingDetector::CheckFlags() const {
  int similarity_measure = flags_ & 0x000f;
  CHECK_MSG(similarity_measure == kTemplateMatchingNcc ||
            similarity_measure == kTemplateMatchingMse ||
            similarity_measure == kTemplateMatchingPsnr ||
            similarity_measure == kTemplateMatchingDssim ||
            similarity_measure == kTemplateMatchingSad,
            "invalid flags");

  int variant = flags_ & 0x00f0;
  CHECK_MSG(variant == kTemplateMatchingBasic ||
            variant == kTemplateMatchingOpencv,
            "invalid flags");
  if (variant == kTemplateMatchingOpencv && (g_flags() & 0x00f0) != kDetector8U)
  {
    DIE_MSG("must use CV_8U format with kTemplateMatchingOpencv");
  }
}

}  // namespace tl
