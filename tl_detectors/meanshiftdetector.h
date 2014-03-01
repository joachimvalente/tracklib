/*!
 * \file meanshiftdetector.h
 * \brief Detector using meanshift or Camshift on HSV histograms.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_MEANSHIFTDETECTOR_H
#define TL_MEANSHIFTDETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"
#include "tl_util/color.h"

namespace tl {

enum MeanshiftVariant {
  TL_MEANSHIFT,
  TL_CAMSHIFT
};

/*!
 * \brief Detector using meanshift or Camshift on HSV histograms.
 */
class MeanshiftDetector : public Detector {
public:
  //--------------------------- Constructor --------------------------
  MeanshiftDetector(const Mat &initial_frame, Rect initial_state);

  //------------------------- Main functions -------------------------
  void Detect();
  std::string ToString() const;

  //------------------------ Public accessors ------------------------
  void set_variant(MeanshiftVariant variant);
  void set_channels_to_use(Channels channels_to_use);
  void set_max_iter(int max_iter);

private:
  //---------------------- Private methods --------------------------
  /*!
   * \brief Compute histogram of the initial template.
   */
  void ComputeTemplateHistogram();

  //----------------------- Internal members ------------------------
  MeanshiftVariant variant_;     //!< Variant ([MEANSHIFT], CAMSHIFT)
  Channels channels_to_use_;     //!< Channels to use (HS, [H], S or GRAY).
  int max_iter_;                 //!< Maximum number of iterations (def. 30).

  int nb_channels_;              //!< Number of channels to use (1 or 2).
  cv::Mat histogram_;            //!< Color histogram of the template.
  const float *ranges_[2];        //!< Ranges used for each channel.
  int cn_[2];                    //!< Array of channel numbers.

  DISALLOW_COPY_AND_ASSIGN(MeanshiftDetector);
};

}  // namespace tl

#endif  // TL_MEANSHIFTDETECTOR_H
