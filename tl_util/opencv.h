/*!
 * \file opencv.h
 * \brief Utility functions on OpenCV objects.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_OPENCV_H
#define TL_OPENCV_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief Check if given rect is inside a frame canvas.
 */
bool IsRectInsideFrame(cv::Rect rect, const cv::Mat &frame);

}  // namespace tl

#endif  // TL_OPENCV_H
