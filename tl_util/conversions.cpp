#include "tl_util/conversions.h"

#include "common.h"

using namespace cv;

namespace tl {
namespace internal {

//------------ Conversions between state-rect and state-matrix ---------------
cv::Rect StateMatToRect(const cv::Mat &mat) {
  CHECK(mat.cols == 1 && mat.rows == 8 && mat.channels() == 1);
  Rect rect;
  rect.x = mat.at<float>(0, 0);
  rect.y = mat.at<float>(0, 1);
  rect.width = mat.at<float>(0, 4);
  rect.height = mat.at<float>(0, 5);
  if (rect.x < 0) rect.x = 0;
  if (rect.y < 0) rect.y = 0;
  if (rect.width < 1) rect.width = 1;
  if (rect.height < 1) rect.height = 1;
  return rect;
}

cv::Mat StateRectToMat(cv::Rect rect) {
  Mat mat = Mat::zeros(4, 1, CV_32F);
  Mat_<float> mat_d(mat);
  mat_d.at<float>(0, 0) = rect.x;
  mat_d.at<float>(0, 1) = rect.y;
  mat_d.at<float>(0, 2) = rect.width;
  mat_d.at<float>(0, 3) = rect.height;
  return mat;
}

cv::Mat StateRectToStandardMat(cv::Rect rect) {
  Mat mat = Mat::zeros(8, 1, CV_32F);
  Mat_<float> mat_d(mat);
  mat_d.at<float>(0, 0) = rect.x;
  mat_d.at<float>(0, 1) = rect.y;
  mat_d.at<float>(0, 4) = rect.width;
  mat_d.at<float>(0, 5) = rect.height;
  return mat;
}

}  // namespace internal
}  // namespace tl
