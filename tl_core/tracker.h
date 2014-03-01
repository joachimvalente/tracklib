/*!
 * \file tracker.h
 * \brief Generic tracker.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TRACKER_H
#define TL_TRACKER_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"
#include "tl_core/filter.h"

namespace tl {

/*!
 * \brief Generic tracker.
 *
 * A tracker is defined as a combination of:
 * - an optional preprocessing step on the input frame (e.g. smoothing, etc.)
 * - an optional online or offline background subtractor,
 * - a detector,
 * - an optional filter,
 * - an optional postprocessing step on the output state (e.g. restrictions on
 * shape, minimum width and height, etc.).
 * .
 *
 * Detector must be specified, by default no filter used.
 * To add pre or post-processing one must derive this class.
 */
class Tracker {
public:
  //---------------------------- Constructor -------------------------
  Tracker();

  //--------------------------- Set components -----------------------
  void set_detector(Detector *detector);
  void set_filter(Filter *filter);

  //--------------------------- Display info -------------------------
  virtual std::string ToString() const;

  //--------------------------- Main function ------------------------
  /*!
   * \brief Track the object in the new frame.
   * \param next_frame Frame where to track the object.
   * \return State of the object.
   * \note If detector or filter were not set, they default to their dummy
   * versions.
   */
  void Track(const cv::Mat &next_frame);

  //---------------------------- Public accessor ------------------------
  cv::Rect state() const;

protected:
  //----------------------- Pre and post-processing ---------------------
  virtual cv::Mat Preprocess(const cv::Mat &frame);
  virtual void Postprocess();

private:
  //--------------------------- Private members ------------------------
  Detector *detector_;                //!< Detector. Not owned.
  Filter *filter_;                    //!< Filter. Not owned.

  cv::Rect state_;                    //!< Current state estimate.

  DISALLOW_COPY_AND_ASSIGN(Tracker);
};

}  // namespace tl

#endif  // TL_TRACKER_H
