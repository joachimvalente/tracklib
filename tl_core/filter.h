/*!
 * \file filter.h
 * \brief Abstract filter.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_FILTER_H
#define TL_FILTER_H

#include <opencv2/core/core.hpp>

namespace tl {

/*!
 * \brief Abstract filter.
 */
class Filter {
public:
  //-------------------------- Constructor ------------------------
  Filter();

  //------------------------- Core functions ----------------------
  /*!
   * \brief Predict state.
   */
  virtual void Predict() = 0;

  /*!
   * \brief Update state estimate.
   * \param z Measurement.
   */
  virtual void Update(const cv::Mat &z) = 0;

  //------------------------- Public accessors --------------------
  /*!
   * \brief Get state estimate.
   */
  const cv::Mat &x() const;

  /*!
   * \brief Get predicted state.
   */
  const cv::Mat &predicted_x() const;

protected:
  //------------------------ Protected members ----------------------
  cv::Mat x_;                 //!< State estimate.
  cv::Mat predicted_x_;       //!< Predicted state.
};

}  // namespace tl

#endif  // TL_FILTER_H
