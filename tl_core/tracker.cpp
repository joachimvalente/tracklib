#include "tl_core/tracker.h"

#include "tl_util/conversions.h"

using namespace tl::internal;

namespace tl {

//--------------------------- Constructor --------------------------
Tracker::Tracker() :
  detector_(nullptr),
  filter_(nullptr) {}

//-------------------------- Set components ------------------------
void Tracker::set_detector(Detector *detector) {
  CHECK_NOTNULL(detector);
  CHECK_MSG(!detector_, "detector has already been set");
  detector_ = detector;
  state_ = detector->state();
}

void Tracker::set_filter(Filter *filter) {
  CHECK_NOTNULL(filter);
  CHECK_MSG(!filter_, "filter has already been set");
  filter_ = filter;
}

//--------------------------- Display info --------------------------
std::string Tracker::ToString() const {
  CHECK_NOTNULL(detector_);
  if (filter_) {
    return detector_->ToString() + " / " + filter_->ToString();
  } else {
    return detector_->ToString();
  }
}

//-------------------------- Main function --------------------------
void Tracker::Track(const Mat &next_frame) {
  CHECK_NOTNULL(detector_);

  cv::Mat frame = Preprocess(next_frame);

  if (filter_ != nullptr) {
    // Predict new position and feed it to the detector.
    filter_->Predict();
    Mat predicted_x = filter_->predicted_x();
    detector_->set_state(StateMatToRect(predicted_x));
  }

  // Detect new state.
  detector_->NextFrame(frame);
  detector_->Detect();

  // Get measurement from core tracker.
  state_ = detector_->state();

  if (filter_ != nullptr) {
    // Feed measurement to Kalman filter and retrieve new state.
    filter_->Update(StateRectToMat(state_));
    state_ = StateMatToRect(filter_->x());
  }

  Postprocess();
}

//------------------------ Public accessor --------------------------
cv::Rect Tracker::state() const {
  CHECK_MSG(detector_, "detector has not been set yet");
  return state_;
}

//----------------------- Pre and post-processing --------------------
cv::Mat Tracker::Preprocess(const Mat &frame) {
  return frame;
}

void Tracker::Postprocess() {}

}  // namespace tl
