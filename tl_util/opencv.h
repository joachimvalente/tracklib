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
   * \brief Get a rect describing the canvas of a frame.
   * \param frame Frame.
   * \return A rect with top-left = (0, 0) and width and height equal to those
   * of the frame.
   */
  cv::Rect FrameCanvas(const cv::Mat &frame);

  /*!
   * \brief Check if given rect is inside a frame canvas.
   * \param frame Frame.
   * \param rect Rect.
   * \return True if `rect` is contained in the frame canvas.
   */
  bool IsRectInsideFrame(const cv::Mat &frame, cv::Rect rect);

}  // namespace tl

#endif  // TL_OPENCV_H
