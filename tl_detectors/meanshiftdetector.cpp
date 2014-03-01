#include "tl_detectors/meanshiftdetector.h"

using namespace cv;

namespace tl {

//--------------------------- Constructor -------------------------
MeanshiftDetector::MeanshiftDetector(const cv::Mat &initial_frame,
                                     cv::Rect initial_state) :
  Detector(initial_frame, initial_state),
  variant_(TL_MEANSHIFT),
  channels_to_use_(TL_H),
  max_iter_(30) {
  // Compute initial template histogram.
  ComputeTemplateHistogram();
}

//-------------------------- Main functions ------------------------
void MeanshiftDetector::Detect() {
  // Compute back projection of the histogram.
  Mat back_proj, converted_frame;
  if (channels() == 3) {
    cvtColor(frame(), converted_frame, CV_RGB2HSV);
  } else {
    converted_frame = frame();
  }
  calcBackProject(&converted_frame, 1, cn_, histogram_, back_proj, ranges_);

  // Apply meanshift or Camshift.
  TermCriteria term_crit(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, max_iter_, 1);
  if (variant_ == TL_MEANSHIFT) {
    Rect s = state();
    cv::meanShift(back_proj, s, term_crit);
    set_state(s);
  } else {
    Rect s = state();
    set_state(cv::CamShift(back_proj, s, term_crit).boundingRect());
  }
}

std::string MeanshiftDetector::ToString() const {
  return (variant_ == TL_MEANSHIFT) ? "meanshift" : "camshift";
}

//----------------------- Public accessors -------------------------
void MeanshiftDetector::set_variant(MeanshiftVariant variant) {
  variant_ = variant;
  ComputeTemplateHistogram();
}

void MeanshiftDetector::set_channels_to_use(Channels channels_to_use) {
  CHECK_MSG(((channels_to_use == TL_H || channels_to_use == TL_S ||
              channels_to_use == TL_HS) && channels() == 3) ||
            (channels_to_use == TL_GRAY && channels() == 1),
            "only TL_H, TL_S, TL_HS (for color) and TL_GRAY (for gray) are "
            "supported");
  channels_to_use_ = channels_to_use;
  ComputeTemplateHistogram();
}

void MeanshiftDetector::set_max_iter(int max_iter) {
  CHECK(max_iter > 0);
  max_iter_ = max_iter;
}

//----------------------- Private methods ----------------------------
void MeanshiftDetector::ComputeTemplateHistogram() {
  // Specify number of channels.
  nb_channels_ = (channels_to_use_ == TL_HS) ? 2 : 1;

  // Specify ranges and number of bins for each channel.
  int bin_sizes[2];
  const float range_h[] = {0, 256};
  const float range_s[] = {0, 180};
  const int bin_size_h = 32;
  const int bin_size_s = 30;
  switch (channels_to_use_) {
    case TL_HS:
      ranges_[0] = range_h;
      ranges_[1] = range_s;
      bin_sizes[0] = bin_size_h;
      bin_sizes[1] = bin_size_s;
      cn_[0] = 0;
      cn_[1] = 1;
      break;
    case TL_H:
    case TL_GRAY:
      ranges_[0] = range_h;
      bin_sizes[0] = bin_size_h;
      cn_[0] = 0;
      break;
    case TL_S:
      ranges_[1] = range_s;
      bin_sizes[1] = bin_size_s;
      cn_[0] = 1;
      break;
    case TL_RGB:
    case TL_HSV:
      DIE;
  }

  // Compute histogram.
  Mat object = initial_frame(initial_state());
  if (channels() == 3) {
    Mat hsv_object;
    Mat mask;
    cvtColor(object, hsv_object, CV_RGB2HSV);

    // Avoid false values due to low light (for color images).
    inRange(hsv_object, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
    calcHist(&hsv_object, 1, cn_, mask, histogram_, nb_channels_, bin_sizes,
             ranges_);
  } else {
    calcHist(&object, 1, cn_, Mat(), histogram_, nb_channels_, bin_sizes, ranges_);
  }

  normalize(histogram_, histogram_, 0, 255, cv::NORM_MINMAX);
}

}  // namespace tl
