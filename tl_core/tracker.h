/*!
 * \file tracker.h
 * \brief Abstract base class of any tracker.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TRACKER_H
#define TL_TRACKER_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief Abstract base class of any tracker.
 * - Initialize with an initial frame, an initial object position/scale.
 * - Feed new frames online via `NextFrame()`.
 * - Run the tracker via `Process()`.
 * - Get new position/scale via `state()` and confidence via
 *   `confidence()`.
 */
class Tracker {
public:
  /*!
   * \brief Constructor.
   * \param initial_frame First frame of the sequence.
   * \param initial_state Initial position/scale of the object to track. Must
   * be contained in the frame canvas.
   */
  Tracker(const cv::Mat &initial_frame, cv::Rect initial_state);

  /*!
   * \brief Feed new frame to the tracker.
   * \param frame The new frame.
   */
  void NextFrame(const cv::Mat &frame);

  /*!
   * \brief Run the tracker.
   */
  virtual void Process() = 0;

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
   * \brief Set the initial position of the state estimate. Can be called:
   * - by child class at the end of its `Process()` implementation,
   * - by extern caller to enforce initial state estimate before calling
   * `Process`.
   * \param state State estimate. Must be contained in the frame canvas.
   * \param confidence Confidence of the estimate.
   */
  void set_state(cv::Rect state, float confidence = 1);

protected:
  /*!
   * \brief Retrieve current frame.
   * \return Current frame.
   */
  const cv::Mat &frame() const;

private:
  const cv::Mat initial_frame_;  //!< First frame of the sequence.
  const cv::Rect initial_state_;  //!< Initial state of the object to track.
  cv::Mat frame_;  //!< Current frame in the sequence.
  cv::Rect state_;  //!< Current estimate of the object state.
  float confidence_;  //!< Confidence \f$\in [0;1]\f$ of the current estimate.
};

}  // namespace tl

#endif  // TL_TRACKER_H
