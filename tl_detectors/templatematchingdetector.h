/*!
 * \file templatematchingdetector.h
 * \brief Template matching detector.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TEMPLATEMATCHINGDETECTOR_H
#define TL_TEMPLATEMATCHINGDETECTOR_H

#include <opencv2/core/core.hpp>

#include "../common.h"
#include "../tl_core/detector.h"

namespace tl {

/*!
 * \brief Variant of template matching algorithm.
 */
enum class TemplateMatchingVariant {
  kBasic       //!< Basic convolution.
};

/*!
 * \brief Flags for template matching.
 */
enum TemplateMatchingFlags {
  // Color channels. For grayscale images only kTemplateMatchingGrayscale is
  // allowed.
  kTemplateMatchingGrayscale = 0x0001,   //!< Use grayscale (intensity).
  kTemplateMatchingHue = 0x0002,         //!< Use 1st channel of HSV.
  kTemplateMatchingSaturation = 0x0003,  //!< Use 2nd channel of HSV.
  kTemplateMatchingHs = 0x0004,          //!< Use first two channels of HSV.
  kTemplateMatchingRgb = 0x0005,         //!< Use RGB values.

  // Measure mode.
  kTemplateMatchingNcc = 0x0010,         //!< Normalized cross-correlation.
  kTemplateMatchingSsd = 0x0010,         //!< Sum of square differences.
  kTemplateMatchingSad = 0x0010,         //!< Sum of absolute differences.
} BITFLAGS(TemplateMatchingFlags);

/*!
 * \brief Template matching detector.
 */
class TemplateMatchingDetector : public Detector {
public:
  /*!
   * \copydoc Detector(const cv::Mat &initial_frame, cv::Rect initial_state)
   * \param variant Variant.
   * \param flags Flags.
   */
  TemplateMatchingDetector(
      const cv::Mat &initial_frame,
      cv::Rect initial_state,
      TemplateMatchingVariant variant = TemplateMatchingVariant::kBasic,
      TemplateMatchingFlags flags = kTemplateMatchingHue);

  /*!
   * \copydoc Detector::Detect()
   */
  virtual void Detect();

private:
  const TemplateMatchingVariant variant_;
  const TemplateMatchingFlags flags_;
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTOR_H
