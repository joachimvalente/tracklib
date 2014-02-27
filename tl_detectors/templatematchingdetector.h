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
  kTemplateMatchingNcc   = 0x0000,         //!< Normalized cross-correlation.
  kTemplateMatchingMse   = 0x0001,         //!< Sum of square differences.
  kTemplateMatchingPsnr  = 0x0002,         //!< Peak signal-to-noise ratio.
  kTemplateMatchingDssim = 0x0003,         //!< Structural dissimilarity.
  kTemplateMatchingSad   = 0x0004,         //!< Sum of absolute differences.

  // Variant.
  kTemplateMatchingBasic = 0x0000,         //!< Basic sliding-window method.

  // GPU.
  kTemplateMatchingUseGpu = 0x0100         //!< Use GPU to compute similarity.
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
   * \param window_size Window size. Must be positive.
   */
  void set_window_size(int window_size);

private:
  const TemplateMatchingFlags flags_;

  cv::Mat template_;                //!< Template from initial frame.

  int window_size_;                 //!< Size of search window.

  DISALLOW_COPY_AND_ASSIGN(TemplateMatchingDetector);
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTOR_H
