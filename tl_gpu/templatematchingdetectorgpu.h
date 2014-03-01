/*!
 * \file templatematchingdetectorgpu.h
 * \brief Detector using template matching on GPU.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TEMPLATEMATCHINGDETECTORGPU_H
#define TL_TEMPLATEMATCHINGDETECTORGPU_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"

namespace tl {

/*!
 * \brief Detector using template matching on GPU.
 */
class TemplateMatchingDetectorGpu : public Detector {
public:
  //----------------------- Constructor -------------------------
  TemplateMatchingDetectorGpu(const cv::Mat &initial_frame,
                              cv::Rect initial_state);

  //----------------------- Main methods ------------------------
  virtual void Detect();

  virtual std::string ToString() const;

  //--------------------- Public accessors ------------------------
  void set_opencv_method(int opencv_method);

private:
  //---------------------- Internal members ------------------------
  const cv::Mat template_;            //!< Template from the initial frame.

  int opencv_method_;                 //!< OpenCV comparison method (CV_TM_*).

  DISALLOW_COPY_AND_ASSIGN(TemplateMatchingDetectorGpu);
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTORGPU_H
