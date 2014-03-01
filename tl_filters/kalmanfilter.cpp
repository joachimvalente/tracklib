#include "tl_filters/kalmanfilter.h"

using namespace cv;

namespace tl {

//-------------------------- Constructors --------------------------
KalmanFilter::KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
                           const cv::Mat &R) :
  F_(F), H_(H), Q_(Q), R_(R), predict_called_(false) {
  CHECK(F.channels() == 1);
  CHECK(H.channels() == 1);
  CHECK(Q.channels() == 1);
  CHECK(R.channels() == 1);

  CHECK(F.rows == F.cols);
  CHECK(Q.rows == Q.cols);
  CHECK(R.rows == R.cols);

  CHECK(F.rows == Q.rows);
  CHECK(H.rows == R.rows);
  CHECK(H.cols == F.rows);

  P_ = cv::Mat::zeros(F.rows, F.rows, CV_32F);
}

KalmanFilter::KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
                           const cv::Mat &R, const cv::Mat &x0) :
  KalmanFilter(F, H, Q, R) {
  Init(x0);
}

//---------------------------- Initialization ----------------------------
void KalmanFilter::Init(const cv::Mat &x0) {
  CHECK(x0.rows == F_.rows && x0.cols == 1);

  x_ = x0;
}

//------------------------------ Update ---------------------------------
void KalmanFilter::Update(const cv::Mat &z) {
  CHECK(z.rows == H_.rows);
  CHECK(predict_called_);
  Mat y = z - H_ * predicted_x_;               // Innovation.
  Mat S = H_ * predicted_P_ * H_.t() + R_;     // Innovation covariance.
  Mat K = predicted_P_ * H_.t() * S.inv();     // Optimal Kalman gain.
  x_ = predicted_x_ + K * y;                   // A posteriori state estimate.
  Mat I = cv::Mat::eye(P_.rows, P_.cols, CV_32F);
  P_ = (I - K * H_) * predicted_P_;            // Updated state covariance.

  predict_called_ = false;
}

//------------------------------- Prediction --------------------------
void KalmanFilter::Predict() {
  predicted_x_ = F_ * x_;                  // A priori state estimate.
  predicted_P_ = F_ * P_ * F_.t() + Q_;    // A priori state covariance.

  predict_called_ = true;
}

}  // namespace tl
