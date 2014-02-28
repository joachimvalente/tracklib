#include "similaritymeasures.h"

#include <cmath>
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
void Normalize(const cv::Mat &in, Mat &out) {
  subtract(in, mean(in), out);
  divide(out, norm(out), out);
}

/*!
 * \f$ NCC(p,t) = \langle \frac{p-\overline{p}}
 * {\| p-\overline{p} \|},
 * \frac{p-\overline{p}}{\| p-\overline{p} \|} \rangle \f$.
 */
float NormalizedCrossCorrelation(const cv::Mat &p, const cv::Mat &t) {
  Mat p_norm;
  Normalize(p, p_norm);

  // Return the sum of dot products over all channels.
  return static_cast<float>(p_norm.dot(t));
}

/*!
 * \f$ NCCI(p,t) = \frac{1}{1 + NCC(p,t)} \f$.
 */
float NormalizedCrossCorrelationInv(const cv::Mat &p, const cv::Mat &t) {
  return 1.0f / (1.0f + NormalizedCrossCorrelation(p, t));
}

/*!
 * \f$ MSE(p,t) = \sqrt{ \frac{1}{N_{row} N_{cols}} \Sigma_i^{N_{rows}}
 * \Sigma_j^{N_{cols}} (p_{ij} - t_{ij})^2 } \f$.
 * The non-exact version only returns the sum.
 */
float MeanSquaredError(const cv::Mat &p, const cv::Mat &t, bool exact) {
  Mat diff;
  absdiff(p, t, diff);                  // |p - t|
  diff.mul(diff);                       // (p - t) ^ 2
  Scalar sums = sum(diff);              // Sum per channel.
  if (exact) {
    return sqrt((sums[0] + sums[1] + sums[2]) /
        (p.rows * p.cols * p.channels()));
  }
  else {
    return sums[0] + sums[1] + sums[2];
  }
}

float MeanSquaredError(const cv::Mat &p, const cv::Mat &t) {
  return MeanSquaredError(p, t, false);
}

/*!
 * \f$ SAD(p,t) = \frac{1}{N_{row} N_{cols}} \Sigma_i^{N_{rows}}
 * \Sigma_j^{N_{cols}} |p_{ij} - t_{ij}| \f$.
 * The non-exact version only returns the sum.
 */
float SumOfAbsoluteDifferences(const cv::Mat &p, const cv::Mat &t, bool exact) {
  Mat diff;
  absdiff(p, t, diff);                  // |p - t|
  Scalar sums = sum(diff);              // Sum per channel.
  if (exact) {
    return (sums[0] + sums[1] + sums[2]) / (p.rows * p.cols * p.channels());
  }
  else {
    return sums[0] + sums[1] + sums[2];
  }
}

float SumOfAbsoluteDifferences(const cv::Mat &p, const cv::Mat &t) {
  return SumOfAbsoluteDifferences(p, t, false);
}

/*!
 * \f$ PSNR(p,t) = 10 \log_{10} \left( \frac{MAX_p^2}{MSE(p,t)} \right) \f$
 * where \f$ MAX_p \f$ is the maximum value over the patch \f$ p \f$.
 */
float PeakSignalToNoiseRatio(const cv::Mat &p, const cv::Mat &t) {
  float mse = MeanSquaredError(p, t, true);
  p.reshape(1);  // Merge all channels to find maximum value.
  double min, max;
  minMaxLoc(p, &min, &max);
  return 10.0f * log10(max * max / mse);
}

/*!
 * \f$ SSIM(p,t) = \frac{(2 \mu_p \mu_t + c_1)(2 \sigma_{pt} + c_2)}
 * {(\mu_p^2 + \mu_t^2 + c_1)(\sigma_p^2 + \sigma_t^2 + c_2)} \f$
 * where \f$ \mu_{p,t} \f$ are the averages of \f$ p \f$ and \f$ t \f$,
 * \f$ \sigma_{p,t}^2 \f$ their variances, and \f$ \sigma_{pt} \f$ their
 * covariance.
 * \f$ c_1 = (k_1 L)^2 \f$ and \f$ c_1 = (k_1 L)^2 \f$ with \f$ L = 255 \f$
 * being the dynamic range and \f$ k_{1,2} \f$ two parameters (by default
 * \f$ k_1 = 0.01 \f$ and \f$ k_2 = 0.03 \f$.
 */
float StructuralSimilarity(const cv::Mat &p, const cv::Mat &t, float c1,
                           float c2) {
  // Compute means, standard deviations and covariances.
  Scalar m_p;   // Mean of p per channel.
  Scalar m_t;   // Mean of t per channel.
  Scalar s_p;   // Standard deviation of p per channel.
  Scalar s_t;   // Standard deviation of t per channel.
  Mat dev_p;    // Deviation of p from its mean.
  Mat dev_t;    // Deviation of t from its mean.
  Scalar s_pt;  // Covariance of p and t per channel.
  meanStdDev(p, m_p, s_p);
  meanStdDev(t, m_t, s_t);
  subtract(p, m_p, dev_p);
  subtract(t, m_t, dev_t);
  vector<Mat> dev_p_c, dev_t_c;
  split(dev_p, dev_p_c);
  split(dev_t, dev_t_c);
  for (int i = 0; i < p.channels(); ++i) {
    s_pt[i] = mean(dev_p_c[i].dot(dev_t_c[i]))[0];
  }

  // Apply SSIM formula.
  float ssim = 0;
  for (int i = 0; i < p.channels(); ++i) {
    ssim += (2 * m_p[i] * m_t[i] + c1) * (2 * s_pt[i] + c2) /
            ((m_p[i] * m_p[i] + m_t[i] * m_t[i] + c1) *
             (s_p[i] * s_p [i] + s_t[i] * s_t[i] + c2));
  }
  return ssim;
}

float StructuralSimilarity(const cv::Mat &p, const cv::Mat &t) {
  return StructuralSimilarity(p, t, 6.5025f, 58.5225f);
}

float StructuralDissimilarity(const cv::Mat &p, const cv::Mat &t) {
  return (1.0f - StructuralSimilarity(p, t)) / 2.0f;
}

}  // namespace tl
