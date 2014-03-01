/*!
 * \file nodetector.h
 * \brief Dummy detector that leaves state estimate and confidence unchanged.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_NODETECTOR_H
#define TL_NODETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"

namespace tl {

/*!
 * \brief Dummy detector.
 */
class NoDetector : public Detector {
public:
  //------------------------- Constructor -----------------------
  NoDetector(const Mat &initial_frame, Rect initial_state);

  //------------------------ Main functions ---------------------
  virtual void Detect();
  virtual std::string ToString() const;

private:
  DISALLOW_COPY_AND_ASSIGN(NoDetector);
};

}  // namespace tl

#endif  // TL_NODETECTOR_H
