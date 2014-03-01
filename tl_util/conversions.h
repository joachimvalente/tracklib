/*!
 * \file conversions.h
 * \brief Convenience conversion functions.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_CONVERSIONS_H
#define TL_CONVERSIONS_H

#include <opencv2/core/core.hpp>

namespace tl {
namespace internal {

//------------ Conversions between state-rect and state-matrix ---------------
cv::Rect StateMatToRect(const cv::Mat &mat);
cv::Mat StateRectToMat(cv::Rect rect);

}  // namespace internal
}  // namespace tl

#endif  // TL_CONVERSIONS_H
