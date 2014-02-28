/*!
 * \file geometry.h
 * \brief Utility functions for geometry.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_GEOMETRY_H
#define TL_GEOMETRY_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief Check if given rect is inside a frame canvas.
 */
bool IsRectInsideFrame(cv::Rect rect, const cv::Mat &frame);

}  // namespace tl

#endif  // TL_GEOMETRY_H
