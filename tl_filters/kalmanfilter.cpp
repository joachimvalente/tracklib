#include "tl_filters/kalmanfilter.h"
#include "tl_util/conversions.h"

using namespace cv;
using namespace tl::internal;

namespace tl {

//-------------------------- Constructors --------------------------
KalmanFilter::KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
                           const cv::Mat &R) {
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

  F.copyTo(F_);
  H.copyTo(H_);
  Q.copyTo(Q_);
  R.copyTo(R_);
  P_ = cv::Mat::zeros(F.rows, F.rows, CV_32F);
}

KalmanFilter::KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
                           const cv::Mat &R, const cv::Mat &x0) :
  KalmanFilter(F, H, Q, R) {
  Init(x0);
}

KalmanFilter::KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
                           const cv::Mat &R, cv::Rect x0) :
  KalmanFilter(F, H, Q, R) {
  Init(StateRectToStandardMat(x0));
}

KalmanFilter::KalmanFilter(float q, float r) {
  Mat I2 = Mat::eye(2, 2, CV_32F);

  Mat F = Mat::zeros(8, 8, CV_32F);
  I2.copyTo(F(Rect(0, 0, 2, 2)));
  I2.copyTo(F(Rect(2, 0, 2, 2)));
  I2.copyTo(F(Rect(2, 2, 2, 2)));
  I2.copyTo(F(Rect(4, 4, 2, 2)));
  I2.copyTo(F(Rect(6, 4, 2, 2)));
  I2.copyTo(F(Rect(6, 6, 2, 2)));

  Mat H = Mat::zeros(4, 8, CV_32F);
  I2.copyTo(H(Rect(0, 0, 2, 2)));
  I2.copyTo(H(Rect(4, 2, 2, 2)));

  Mat Q = Mat::zeros(8, 8, CV_32F);
  Mat_<float> Q_d(Q);
  Q_d.at<float>(0, 0) = Q_d.at<float>(1, 1) = q;
  Q_d.at<float>(2, 2) = Q_d.at<float>(3, 3) = q;
  Q_d.at<float>(4, 4) = Q_d.at<float>(5, 5) = q;
  Q_d.at<float>(6, 6) = Q_d.at<float>(7, 7) = q;

  Mat R = Mat::zeros(4, 4, CV_32F);
  Mat_<float> R_d(R);
  R_d.at<float>(0, 0) = R_d.at<float>(1, 1) = r;
  R_d.at<float>(2, 2) = R_d.at<float>(3, 3) = r;

  F.copyTo(F_);
  H.copyTo(H_);
  Q.copyTo(Q_);
  R.copyTo(R_);
  P_ = cv::Mat::zeros(F.rows, F.rows, CV_32F);
}

KalmanFilter::KalmanFilter(const cv::Mat &x0, float q, float r) :
  KalmanFilter(q, r) {
  Init(x0);
}

KalmanFilter::KalmanFilter(cv::Rect x0, float q, float r) :
  KalmanFilter(q, r) {
  Init(StateRectToStandardMat(x0));
}

//---------------------------- Initialization ----------------------------
void KalmanFilter::Init(const cv::Mat &x0) {
  CHECK(x0.rows == F_.rows && x0.cols == 1);

  x_ = x0;
}

//------------------------------ Update ---------------------------------
void KalmanFilter::Update(const cv::Mat &z) {
  CHECK(z.rows == H_.rows);
  Mat y = z - H_ * predicted_x_;               // Innovation.
  Mat S = H_ * predicted_P_ * H_.t() + R_;     // Innovation covariance.
  Mat K = predicted_P_ * H_.t() * S.inv();     // Optimal Kalman gain.
  x_ = predicted_x_ + K * y;                   // A posteriori state estimate.
  Mat I = cv::Mat::eye(P_.rows, P_.cols, CV_32F);
  P_ = (I - K * H_) * predicted_P_;            // Updated state covariance.
}

//------------------------------- Prediction --------------------------
void KalmanFilter::Predict() {
  predicted_x_ = F_ * x_;                  // A priori state estimate.
  predicted_P_ = F_ * P_ * F_.t() + Q_;    // A priori state covariance.
}

//------------------------------ Description --------------------------
std::string KalmanFilter::ToString() const {
  return "Kalman filter";
}

}  // namespace tl
