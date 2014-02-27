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
  window_size_(20) {
  INFO(ToString());

  // Save model.
  template_ = Detector::initial_frame()(initial_state);

  // Normalization of the template (if using NCC).
  if ((flags_ & 0x000f) == kTemplateMatchingNcc) {
    Normalize(template_, template_);
  }
}

void TemplateMatchingDetector::Detect() {
  // Select distance function.
  SimilarityMeasure distance_function;
  switch (flags_ & 0x00f0) {
    case kTemplateMatchingNcc:
      distance_function = NormalizedCrossCorrelationInv;
      break;
    case kTemplateMatchingMse:
      distance_function = MeanSquareError;
      break;
    case kTemplateMatchingPsnr:
      distance_function = PeakSignalToNoiseRatio;
      break;
    case kTemplateMatchingDssim:
      distance_function = StructuralDissimilarity;
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
  int min_x = std::max(0, state().x - window_size_);
  int max_x = std::min(width() - template_.cols, state().x + window_size_);
  int min_y = std::max(0, state().y - window_size_);
  int max_y = std::min(height() - template_.rows, state().y + window_size_);
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

std::string TemplateMatchingDetector::ToString() const {
  string variant, color_mode, similarity_measure;
  switch (flags_ & 0x00f0) {
    case kTemplateMatchingBasic:
      variant = "basic";
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

  return "template matching detector [variant=" + variant + " color_mode=" +
      color_mode + " similarity_measure=" + similarity_measure +
      " windows_size=" + to_string(window_size_) + "]";
}

int TemplateMatchingDetector::window_size() const {
  return window_size_;
}

void TemplateMatchingDetector::set_window_size(int window_size) {
  if (window_size > 0) {
    window_size_ = window_size;
  } else {
    WARNING("window_size must be positive");
  }
}

}  // namespace tl
