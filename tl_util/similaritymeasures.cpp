#include "similaritymeasures.h"

#include <vector>

#include <opencv2/highgui/highgui.hpp>

#include "../common.h"

using namespace cv;
using namespace std;

namespace tl {

/*!
 * Normalize by subtracting the mean and dividing by the standard deviation
 * and the square root of the number of elements, i.e. dividing by the norm
 * of the deviation to the mean.
 */
void Normalize(const cv::Mat &in, cv::Mat &out) {
  subtract(in, mean(in), out);
  divide(out, norm(out), out);
}

float NormalizedCrossCorrelation(const cv::Mat &p, const cv::Mat &t) {
  Mat p_norm;
  Normalize(p, p_norm);

  // Return the sum of dot products over all channels.
  return p_norm.dot(t);
}

float NormalizedCrossCorrelationInv(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f / (NormalizedCrossCorrelation(p, t) + 1.0f);
}

float MeanSquareError(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f;
}

float SumOfAbsoluteDifferences(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f;
}

float PeakSignalToNoiseRatio(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f;
}

float StructuralSimilarity(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f;
}

float StructuralDissimilarity(const cv::Mat &p, const cv::Mat &t) {
  return (1.0f - StructuralSimilarity(p, t)) / 2.0f;
}

}  // namespace tl
