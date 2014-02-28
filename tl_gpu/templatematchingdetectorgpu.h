/*!
 * \file templatematchingdetector.h
 * \brief GPU implementation of template matching detectors.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TEMPLATEMATCHINGDETECTORGPU_H
#define TL_TEMPLATEMATCHINGDETECTORGPU_H

#include <opencv2/core/core.hpp>
#include <opencv2/gpu/gpu.hpp>

#include "../tl_core/detector.h"

namespace tl {

class TemplateMatchingDetectorGpu : public Detector {
public:
  /*!
   * \copydoc Detector(const cv::Mat&, cv::Rect, DetectorFlags)
   */
  TemplateMatchingDetectorGpu(const cv::Mat &initial_frame,
                              cv::Rect initial_state,
                              DetectorFlags g_flags = kDetectorGrayscale |
                                                      kDetector8U);

  /*!
   * \copydoc Detector::Detect()
   */
  virtual void Detect();

  /*!
   * \copydoc Detector::ToString()
   */
  virtual std::string ToString() const;

  /*!
   * \brief Get OpenCV comparison method.
   * \return OpenCV comparison method.
   */
  int opencv_method() const;

  /*!
   * \brief Set OpenCV comparison method.
   * \param opencv_method OpenCV comparison method.
   */
  void set_opencv_method(int opencv_method);

private:
  int opencv_method_;              //!< OpenCV comparison method.

  cv::Mat template_;               //!< Template from the initial frame.

  DISALLOW_COPY_AND_ASSIGN(TemplateMatchingDetectorGpu);
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTORGPU_H
