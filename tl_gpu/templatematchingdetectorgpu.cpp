#include "tl_gpu/templatematchingdetectorgpu.h"

#include <opencv2/gpu/gpu.hpp>

using namespace cv;

namespace tl {

//------------------------- Constructor -------------------------
TemplateMatchingDetectorGpu::TemplateMatchingDetectorGpu(
    const cv::Mat &initial_frame, cv::Rect initial_state) :
  Detector(initial_frame, initial_state),
  template_(initial_frame(initial_state).clone()),
  opencv_method_(CV_TM_SQDIFF) {
  // Initialize CUDA.
  gpu::setDevice(0);
}

//------------------------- Main methods ------------------------
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
  return "template matching detector (gpu)";
}

//------------------------ Public accessors -------------------------
void TemplateMatchingDetectorGpu::set_opencv_method(int opencv_method) {
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
