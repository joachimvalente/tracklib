/*!
 * \file dummydetector.h
 * \brief Dummy detector that leaves state estimate and confidence unchanged.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_DUMMYDETECTOR_H
#define TL_DUMMYDETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"

namespace tl {

/*!
 * \brief Dummy detector.
 */
class DummyDetector : public Detector {
public:
  DummyDetector(const cv::Mat &inital_frame, cv::Rect initial_state);

  virtual void Detect();

  virtual std::string ToString() const;

private:
  DISALLOW_COPY_AND_ASSIGN(DummyDetector);
};

}  // namespace tl

#endif  // TL_DUMMYDETECTOR_H
