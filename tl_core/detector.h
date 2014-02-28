/*!
 * \file detector.h
 * \brief Abstract base class of any detector.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_DETECTOR_H
#define TL_DETECTOR_H

#include <string>

#include <opencv2/core/core.hpp>

#include "../common.h"

namespace tl {

enum DetectorFlags {
  // Color channels. For grayscale images only kDetectorGrayscale is allowed.
  kDetectorGrayscale = 0x0000,   //!< Use grayscale (intensity).
  kDetectorHue = 0x0001,         //!< Use 1st channel of HSV.
  kDetectorSaturation = 0x0002,  //!< Use 2nd channel of HSV.
  kDetectorHs = 0x0003,          //!< Use first two channels of HSV.
  kDetectorRgb = 0x0004,         //!< Use RGB values.

  // Value type.
  kDetector32F = 0x0000,         //!< Convert to 32-bit float.
  kDetector8U = 0x0010,          //!< Leave as 8-bit unsigned integers.

  // No value.
  kDetectorNone = 0x0000
} BITFLAGS(DetectorFlags);

/*!
 * \brief Abstract base class of any detector.
 * - Initialize with an initial frame and an initial object position/scale.
 * - Feed new frames online via `NextFrame()`.
 * - Run the detector via `Detect()`.
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
   * \param g_flags Generic detector flags.
   */
  Detector(const cv::Mat &initial_frame, cv::Rect initial_state,
           DetectorFlags g_flags = kDetectorGrayscale | kDetector32F);

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
   * \brief Set the position of the state estimate. To be called:
   * - by child class at the end of its `Detect()` implementation,
   * - by extern caller to enforce initial state estimate before calling
   * `Detect()`.
   * \param state State estimate. Must be contained in the frame canvas and
   * non empty.
   * \param confidence Confidence of the estimate.
   */
  void set_state(cv::Rect state, float confidence = 1);

  /*!
   * \copydoc Detector::set_state(cv::Rect, float)
   * \param tl Top left corner of the new state (size unchanged).
   */
  void set_state(cv::Point tl, float confidence = 1);

  /*!
   * \brief Give a description of the detector.
   * \return Description of the detector.
   */
  virtual std::string ToString() const;

protected:
  /*!
   * \brief Get the initial frame.
   * \return The initial frame.
   */
  const cv::Mat &initial_frame() const;

  /*!
   * \brief Get the initial state.
   * \return The initial state.
   */
  const cv::Rect &initial_state() const;

  /*!
   * \brief Get the current frame.
   * \return The current frame.
   */
  const cv::Mat &frame() const;

  /*!
   * \brief Get width of sequence frames.
   * \return Width of sequence frames.
   */
  const int width() const;

  /*!
   * \brief Get height of sequence frames.
   * \return Height of sequence frames.
   */
  const int height() const;

  /*!
   * \brief Get number of channels.
   * \return Number of channels in sequence frames.
   */
  const int channels() const;

  /*!
   * \brief Get a measure of confidence of the estimate.
   * \return The confidence or 1 if the tracker does not give a measure of
   * confidence.
   */
  float confidence() const;

  /*!
   * \brief Get generic detector flags.
   * \return Generic detector flags.
   */
  DetectorFlags g_flags() const;

private:
  /*!
   * \brief Convert frame to correct color space with 1, 2 or 3 channels,
   * and with type CV_32F if needed.
   * \param in Frame to convert.
   * \param out Output.
   */
  void ConvertColorAndType(const cv::Mat &in, cv::Mat &out);

  /*!
   * \brief Convert frame to correct color space with 1, 2 or 3 channels,
   * and with type CV_32F if needed.
   * \param in Frame to convert.
   * \return Output.
   */
  cv::Mat ConvertColorAndType(const cv::Mat &in);

  /*!
   * \brief Check for flags validity.
   */
  void CheckFlags() const;

  const int width_;               //!< Width of frames.
  const int height_;              //!< Height of frames.
  const int channels_;            //!< Number of channels.
  const DetectorFlags g_flags_;   //!< Generic detector flags.

  const cv::Mat initial_frame_;   //!< First frame of the sequence.
  const cv::Rect initial_state_;  //!< Initial state of the object to detect.

  cv::Mat frame_;                 //!< Current frame in the sequence.
  cv::Rect state_;                //!< Current estimate of the object state.
  float confidence_;              //!< Confidence \f$\in [0;1]\f$ of current
                                  //!  state estimate.
};

}  // namespace tl

#endif  // TL_DETECTOR_H
