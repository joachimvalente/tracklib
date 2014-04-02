/*!
 * \file kalmanfilter.h
 * \brief Kalman filter.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_KALMANFILTER_H
#define TL_KALMANFILTER_H

#include <opencv2/core/core.hpp>

#include "common.h"
#include "tl_core/filter.h"

namespace tl {

class KalmanFilter : public Filter {
public:
  //------------------------- Constructors ------------------------
  /*!
   * \brief Constructor.
   * \param F Dynamic model.
   * \param H Observation model.
   * \param Q Covariance of process noise.
   * \param R Covariance of observation noise.
   */
  KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
               const cv::Mat &R);

  /*!
   * \copydoc KalmanFilter(const cv::Mat&, const cv::Mat&, const cv::Mat&,
   * const cv::Mat&);
   * \param x0 Initial state.
   */
  KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
               const cv::Mat &R,
               const cv::Mat &x0);

  /*!
   * \copydoc KalmanFilter(const cv::Mat&, const cv::Mat&, const cv::Mat&,
   * const cv::Mat&);
   * \param x0 Initial state.
   */
  KalmanFilter(const cv::Mat &F, const cv::Mat &H, const cv::Mat &Q,
               const cv::Mat &R,
               cv::Rect x0);

  /*!
   * \brief Construct a Kalman filter for standard tracking model.
   *
   * \f$ F = \left( \begin{array}{cccc} I_2 & I_2 & 0 & 0 \\ 0 & I_2 & 0 & 0 \\
   * 0 & 0 & I_2 & I_2 \\ 0 & 0 & 0 & I_2 \end{array} \right), \ \ \
   * H = \left( \begin{array}{cccc} I_2 & 0 & 0 & 0 \\ 0 & 0 & I_2 & 0
   * \end{array} \right), \ \ \
   * Q = q I_8 \ \ \f$ and \f$ \ \ R = r I_4 \f$.
   *
   * State \f$x\f$ must be \f$[ x, \ y, \ v_x, \ v_y, \ w, \ h, \ v_w, \
   * v_h, ]^\top\f$ with \f$v_p = \frac{dp}{dt} \f$.
   * \param q
   * \param r
   */
  KalmanFilter(float q = 0.015f, float r = 12.0f);

  /*!
   * \copydoc KalmanFilter(float, float)
   * \param x0 Initial state.
   */
  KalmanFilter(const cv::Mat &x0, float q = 0.015f, float r = 12.0f);

  /*!
   * \copydoc KalmanFilter(float, float)
   * \param state Initial state.
   */
  KalmanFilter(cv::Rect x0, float q = 0.015f, float r = 12.0f);

  //------------------------- Initialization ----------------------
  /*!
   * \brief Initialize with initial state.
   */
  void Init(const cv::Mat &x0);

  //------------------------- Core functions ----------------------
  /*!
   * \copydoc Filter::Predict()
   */
  virtual void Predict();

  /*!
   * \copydoc Filter::Update(const cv::Mat&)
   */
  virtual void Update(const cv::Mat &z);

  /*!
   * \copydoc Filter::ToString()
   */
  virtual std::string ToString() const;

private:
  //------------------------ Private members ----------------------
  cv::Mat F_;                 //!< Dynamic model.
  cv::Mat H_;                 //!< Observation model.
  cv::Mat Q_;                 //!< Covariance of process noise.
  cv::Mat R_;                 //!< Covariance of observation noise.
  cv::Mat P_;                 //!< State covariance estimate.
  cv::Mat predicted_P_;       //!< Predicted covariance.

  DISALLOW_COPY_AND_ASSIGN(KalmanFilter);
};

}  // namespace tl

#endif  // TL_KALMANFILTER_H
