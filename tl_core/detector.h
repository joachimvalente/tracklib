/*!
 * \file detector.h
 * \brief Abstract detector class.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_DETECTOR_H
#define TL_DETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "common.h"

using namespace cv;

namespace tl {

/*!
 * \brief Abstract detector.
 * - Initialize with an initial frame and an initial state (= OpenCV Rect).
 * - Feed new frames online via `NextFrame()`.
 * - Run the detector via `Detect()`.
 * - Get new state via `state()` and confidence via `confidence()`.
 * .
 * Children classes must implement the `Detect()` method.
 * All fed images must be of same dimensions and type. They must be grayscale or
 * RGB and have a depth of CV_8U or CV_32F.
 */
class Detector {
public:
  //--------------------- Constructor -----------------
  /*!
   * \brief Constructor.
   * \param initial_frame First frame of the sequence.
   * \param initial_state Initial state of the object to detect. Must be non
   * empty and contained in the frame canvas.
   */
  Detector(const cv::Mat &initial_frame, cv::Rect initial_state);

  //--------------------- Main methods ------------------
  /*!
   * \brief Feed new frame to the detector.
   * \param frame The new frame.
   */
  void NextFrame(const cv::Mat &frame);

  /*!
   * \brief Run the detection task.
   */
  virtual void Detect() = 0;

  /*!
   * \brief Get a textual description of the detector.
   */
  virtual std::string ToString() const;

  //----------------------- Accessors -----------------------
  cv::Rect state() const;

  /*!
   * \brief Set the state estimate. Can be called be child class
   * at the end of detection, or by extern caller before calling `Detect()` to
   * enforce the initial state.
   * \param confidence Confidence measure \f$ \in [0; 1] \f$.
   */
  void set_state(cv::Rect state, float confidence = 1);

  /*!
   * \copydoc Detector::set_state(cv::Rect, float)
   * \param tl Top left corner of the new state (width and height unchanged).
   */
  void set_state(cv::Point tl, float confidence = 1);

  float confidence() const;

  /*!
   * \param confidence Confidence measure \f$ \in [0; 1] \f$.
   */
  void set_confidence(float confidence);

protected:
  //---------------- Protected accessors -------------------
  const cv::Mat &initial_frame() const;
  Mat initial_frame(cv::Rect roi) const;
  const cv::Rect &initial_state() const;
  const cv::Mat &frame() const;
  int width() const;
  int height() const;
  int channels() const;
  int depth() const;

private:
  //-------------------- Internal members ---------------------
  int width_;                     //!< Width of frames.
  int height_;                    //!< Height of frames.
  int channels_;                  //!< Number of channels.
  int depth_;                     //!< Depth.

  cv::Mat initial_frame_;         //!< First frame of the sequence.
  cv::Rect initial_state_;        //!< Initial state of the object to detect.

  cv::Mat frame_;                 //!< Current frame in the sequence.
  cv::Rect state_;                //!< Current estimate of the object state.
  float confidence_;              //!< Confidence \f$\in [0;1]\f$ of current
                                  //!  state estimate.
};

}  // namespace tl

#endif  // TL_DETECTOR_H
