/*!
 * \file similaritymeasures.h
 * \brief Functions for measuring similarity between patches.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_SIMILARITYMEASURES_H
#define TL_SIMILARITYMEASURES_H

#include <opencv2/core/core.hpp>

namespace tl {

typedef float (*SimilarityMeasure)(const cv::Mat&, const cv::Mat&);

/*!
 * \brief Normalize. Process each channel separately.
 * \param in Matrix to normalize. Must be in CV_32FCx format.
 * \param out Normalized matrix.
 * \note No safety checks for efficiency reasons.
 */
void Normalize(const cv::Mat &in, cv::Mat &out);

/*!
 * \brief Compute normalized cross correlation (NCC).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p` and be prenormalized.
 * \return NCC between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float NormalizedCrossCorrelation(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute normalized cross correlation (NCC) and inverts it to obtain
 * a measure of dissimilarity.
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p` and be prenormalized.
 * \return Inverted NCC between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float NormalizedCrossCorrelationInv(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute mean square error (MSE).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p`.
 * \return MSE between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float MeanSquareError(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute sum of absolute differences (SAD).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p`.
 * \return SAD between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float SumOfAbsoluteDifferences(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute peak signal-to-noise ratio (PSNR).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p`.
 * \return PSNR between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float PeakSignalToNoiseRatio(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute structural similarity (SSIM).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p`.
 * \return SSIM between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float StructuralSimilarity(const cv::Mat &p, const cv::Mat &t);

/*!
 * \brief Compute structural dissimilarity (DSSIM).
 * \param p Patch. Must be in CV_32FCx format.
 * \param t Template. Must have same dimensions as `p`.
 * \return DSSIM between the two patches.
 * \note No safety checks for efficiency reasons.
 */
float StructuralDissimilarity(const cv::Mat &p, const cv::Mat &t);

}  // namespace tl

#endif  // TL_SIMILARITYMEASURES_H
