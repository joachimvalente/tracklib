/*!
 * \file detector.h
 * \brief Abstract base class of any detector.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_DETECTOR_H
#define TL_DETECTOR_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief Abstract base class of any detector.
 * - Initialize with an initial frame and an initial object position/scale.
 * - Feed new frames online via `NextFrame()`.
 * - Run the detector via `Process()`.
 * - Get new position/scale via `state()` and confidence via
 *   `confidence()`.
 */
class Detector {
public:
  /*!
   * \brief Constructor.
   * \param initial_frame First frame of the sequence.
   * \param initial_state Initial position/scale of the object to detect. Must
   * be contained in the frame canvas and non empty.
   */
  Detector(const cv::Mat &initial_frame, cv::Rect initial_state);

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
   * \brief Get the estimate of the state.
   * \return The estimate of the state.
   */
  cv::Rect state() const;

  /*!
   * \brief Get a measure of confidence of the estimate.
   * \return The confidence or 1 if the tracker does not give a measure of
   * confidence.
   */
  float confidence() const;

  /*!
   * \brief Set the initial position of the state estimate. To be called:
   * - by child class at the end of its `Detect()` implementation,
   * - by extern caller to enforce initial state estimate before calling
   * `Detect()`.
   * \param state State estimate. Must be contained in the frame canvas and
   * non empty.
   * \param confidence Confidence of the estimate.
   */
  void set_state(cv::Rect state, float confidence = 1);

protected:
  /*!
   * \brief Accessor for the frame.
   * \return The current frame.
   */
  const cv::Mat &frame() const;

  /*!
   * \brief Accessor for the width.
   * \return Width of sequence frames.
   */
  const int width() const;

  /*!
   * \brief Accessor for the height.
   * \return Height of sequence frames.
   */
  const int height() const;

  /*!
   * \brief Accessor for number of channels.
   * \return Number of channels in sequence frames.
   */
  const int channels() const;

private:
  const cv::Mat initial_frame_;  //!< First frame of the sequence.
  const cv::Rect initial_state_;  //!< Initial state of the object to detect.
  cv::Mat frame_;  //!< Current frame in the sequence.
  cv::Rect state_;  //!< Current estimate of the object state.
  float confidence_;  //!< Confidence \f$\in [0;1]\f$ of the current estimate.

  const int width_;     //!< Width of frames.
  const int height_;    //!< Height of frames.
  const int channels_;  //!< Number of channels.
};

}  // namespace tl

#endif  // TL_DETECTOR_H
