/*!
 * \file templatematchingdetector.h
 * \brief Detector using template matching.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TEMPLATEMATCHINGDETECTOR_H
#define TL_TEMPLATEMATCHINGDETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/detector.h"

namespace tl {

/*!
 * \brief Detector using template matching.
 */
class TemplateMatchingDetector : public Detector {
public:
  //----------------------- Constructor -------------------------
  TemplateMatchingDetector(const Mat &initial_frame, Rect initial_state);

  //----------------------- Main methods ------------------------
  virtual void Detect();

  virtual std::string ToString() const;

  //--------------------- Public accessors ------------------------
  void set_opencv_method(int opencv_method);

private:
  //---------------------- Internal members ------------------------
  cv::Mat template_;            //!< Template from the initial frame.

  int opencv_method_;                 //!< OpenCV comparison method (CV_TM_*)
                                      //!  [def. CV_TM_SQDIFF].

  DISALLOW_COPY_AND_ASSIGN(TemplateMatchingDetector);
};

}  // namespace tl

#endif  // TL_TEMPLATEMATCHINGDETECTOR_H
