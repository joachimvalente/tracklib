/*!
 * \file dummydetector.h
 * \brief Dummy detector that leaves state estimate and confidence unchanged.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_DUMMYDETECTOR_H
#define TL_DUMMYDETECTOR_H

#include <opencv2/core/core.hpp>

#include "../common.h"
#include "../tl_core/detector.h"

namespace tl {

/*!
 * \brief Dummy detector that leaves state estimate and confidence unchanged.
 */
class DummyDetector : public Detector {
public:
  /*!
   * \copydoc Detector::Detector(const cv::Mat &inital_frame,
   * cv::Rect initial_state)
   */
  DummyDetector(const cv::Mat &inital_frame, cv::Rect initial_state);

  /*!
   * \copydoc Detector::Detect()
   */
  virtual void Detect();

private:
  DISALLOW_COPY_AND_ASSIGN(DummyDetector);
};

}  // namespace tl

#endif  // TL_DUMMYDETECTOR_H
