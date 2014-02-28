/*!
 * \file templatematchingdetector.h
 * \brief Template matching detector.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TEMPLATEMATCHINGDETECTOR_H
#define TL_TEMPLATEMATCHINGDETECTOR_H

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include "../common.h"
#include "../tl_core/detector.h"

namespace tl {

/*!
 * \brief Flags for template matching.
 */
enum TemplateMatchingFlags {
  // Similarity measure.
  kTemplateMatchingNcc    = 0x0000,        //!< Normalized cross-correlation.
  kTemplateMatchingMse    = 0x0001,        //!< Mean squared error.
  kTemplateMatchingPsnr   = 0x0002,        //!< Peak signal-to-noise ratio.
  kTemplateMatchingDssim  = 0x0003,        //!< Structural dissimilarity.
  kTemplateMatchingSad    = 0x0004,        //!< Sum of absolute differences.

  // Variant.
  kTemplateMatchingBasic  = 0x0000,        //!< Basic sliding-window method.
  kTemplateMatchingOpencv = 0x0010,        //!< Use OpenCV built-in function
                                           //!  (set via ~set_opencv_method()`).

  // No value.
  kTemplateMatchingNone = 0x0000
} BITFLAGS(TemplateMatchingFlags);

/*!
 * \brief Template matching detector.
 */
class TemplateMatchingDetector : public Detector {
public:
  /*!
   * \copydoc Detector(const cv::Mat&, cv::Rect, DetectorFlags)
   * \param flags Template matching flags.
   */
  TemplateMatchingDetector(
      const cv::Mat &initial_frame,
      cv::Rect initial_state,
      DetectorFlags g_flags = kDetectorGrayscale,
      TemplateMatchingFlags flags = kTemplateMatchingNcc |
                                    kTemplateMatchingBasic);

  /*!
   * \copydoc Detector::Detect()
   */
  virtual void Detect();

  /*!
   * \copydoc Detector::ToString()
   */
  virtual std::string ToString() const;

  /*!
   * \brief Get window size.
   * \return Window size.
   */
  int window_size() const;

  /*!
   * \brief Set window size.
   * \param window_size Window size. Must be positive, or 0 to search the entire
   * image.
   */
  void set_window_size(int window_size);

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
  /*!
   * \brief Detect using basic sliding-window method.
   */
  void DetectBasic();

  /*!
   * \brief Detect using OpenCV built-in `matchTemplate()` function.
   */
  void DetectOpencv();

  /*!
   * \brief Check for flags validity.
   */
  void CheckFlags() const;

  const TemplateMatchingFlags flags_;

  cv::Mat template_;                //!< Template from initial frame.

  int window_size_;                 //!< Size of search window.

  int opencv_method_;               //!< OpenCV comparison method.

  DISALLOW_COPY_AND_ASSIGN(TemplateMatchingDetector);
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTOR_H
