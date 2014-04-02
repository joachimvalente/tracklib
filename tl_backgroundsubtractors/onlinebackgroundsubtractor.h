/*!
 * \file onlinebackgroundsubtractor.h
 * \brief Online Background Subtractor.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_ONLINEBACKGROUNDSUBTRACTOR_H
#define TL_ONLINEBACKGROUNDSUBTRACTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

#include "common.h"
#include "tl_core/backgroundsubtractor.h"

namespace tl {

enum BackgroundSubtractionMethod {
  TL_GMG,       //!< Wrapper of OpenCV BackgroundSubtractorGMG.
  TL_MOG,       //!< Wrapper of OpenCV BackgroundSubtractorMOG.
  TL_MOG2       //!< Wrapper of OpenCV BackgroundSubtractorMOG2.
};

class OnlineBackgroundSubtractor : public BackgroundSubtractor {
public:
  //-------------------- Constructor and destructor -------------------
  OnlineBackgroundSubtractor(const cv::Mat &initial_frame,
                             BackgroundSubtractionMethod method);

  ~OnlineBackgroundSubtractor();

private:
  //----------------------- Compute background ------------------------
  void Compute();

  cv::BackgroundSubtractor *opencv_bgs_;

  DISALLOW_COPY_AND_ASSIGN(OnlineBackgroundSubtractor);
};

}  // namespace tl

#endif  // TL_ONLINEBACKGROUNDSUBTRACTOR_H
