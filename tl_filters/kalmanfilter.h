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

private:
  //------------------------ Private members ----------------------
  const cv::Mat F_;           //!< Dynamic model.
  const cv::Mat H_;           //!< Observation model.
  const cv::Mat Q_;           //!< Covariance of process noise.
  const cv::Mat R_;           //!< Covariance of observation noise.
  cv::Mat P_;                 //!< State covariance estimate.
  cv::Mat predicted_P_;       //!< Predicted covariance.
  bool predict_called_;       //!< Whether Predict() was called.

  DISALLOW_COPY_AND_ASSIGN(KalmanFilter);
};

}  // namespace tl

#endif  // TL_KALMANFILTER_H
