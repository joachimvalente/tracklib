/*!
 * \file backgroundsubtractor.h
 * \brief Abstract background subtractor class.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_BACKGROUNDSUBTRACTOR_H
#define TL_BACKGROUNDSUBTRACTOR_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief The BackgroundSubtractor class
 * The background model is updated each time a new frame is fed using
 * `NextFrame()` and can be retrieved as an 8-bit unsigned int mask via
 * `background()`. The complement can be retrieved via the pseudo-accessor
 * `foreground()`.
 */
class BackgroundSubtractor {
public:
  //---------------------------- Constructor -------------------------
  BackgroundSubtractor(const cv::Mat &initial_frame);

  //-------------------------- Compute background --------------------
  /*!
   * \brief Feed next frame to the background subtractor.
   * \param frame The new frame.
   */
  void NextFrame(const cv::Mat &frame);

  //---------------------------- Retrieve foreground -----------------
  /*!
   * \brief Segment the foreground from the image (background appears black).
   */
  cv::Mat GetForeground() const;

  //----------------------------- Accessors --------------------------
  /*!
   * \brief Get the background mask.
   */
  const cv::Mat &background() const;

  /*!
   * \brief Get the foreground mask.
   */
  cv::Mat foreground() const;

protected:
  /*!
   * \brief Compute the background mask. Called by `NextFrame()`.
   */
  virtual void Compute() = 0;

  cv::Mat frame_;                    //!< The current frame.
  cv::Mat background_;               //!< The current background mask.
};

}

#endif  // TL_BACKGROUNDSUBTRACTOR_H
