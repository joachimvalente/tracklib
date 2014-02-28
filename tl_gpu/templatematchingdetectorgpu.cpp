#include "templatematchingdetectorgpu.h"

using namespace cv;

namespace tl {

TemplateMatchingDetectorGpu::TemplateMatchingDetectorGpu(
    const cv::Mat &initial_frame,
    cv::Rect initial_state,
    DetectorFlags g_flags) :
  Detector(initial_frame, initial_state, g_flags),
  opencv_method_(CV_TM_SQDIFF) {
  INFO(ToString());
  template_ = Detector::initial_frame()(initial_state);
}

void TemplateMatchingDetectorGpu::Detect() {
  gpu::GpuMat template_g;
  template_g.upload(template_);

  gpu::GpuMat frame_g;
  frame_g.upload(frame());

  gpu::GpuMat result_g;

  gpu::matchTemplate(frame_g, template_g, result_g, CV_TM_CCORR);
  Point location;
  if (opencv_method_ == CV_TM_SQDIFF || opencv_method_ == CV_TM_SQDIFF_NORMED) {
    // Locate min.
    gpu::minMaxLoc(result_g, nullptr, nullptr, &location, nullptr);
  } else {
    // Locate max.
    gpu::minMaxLoc(result_g, nullptr, nullptr, nullptr, &location);
  }
  set_state(location);
}

std::string TemplateMatchingDetectorGpu::ToString() const {
  return "Template matching detector on GPU";
}

int TemplateMatchingDetectorGpu::opencv_method() const {
  return opencv_method_;
}

void TemplateMatchingDetectorGpu::set_opencv_method(int opencv_method) {
  CHECK_MSG(opencv_method == CV_TM_CCOEFF ||
            opencv_method == CV_TM_CCOEFF_NORMED ||
            opencv_method == CV_TM_CCORR ||
            opencv_method == CV_TM_CCORR_NORMED ||
            opencv_method == CV_TM_SQDIFF ||
            opencv_method == CV_TM_SQDIFF_NORMED,
            "invalid OpenCV comparison method");
  opencv_method_ = opencv_method;
}

}  // namespace tl
