/*!
 * \file color.h
 * \brief Utility functions to handle color spaces and conversions.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_COLOR_H
#define TL_COLOR_H

namespace tl {

/*!
 * \brief Channel combination.
 */
enum Channels {
  // For RGB.
  TL_RGB,

  // For HSV.
  TL_HSV,
  TL_HS,
  TL_H,
  TL_S,

  // For grayscale.
  TL_GRAY
};

}  // namespace tl

#endif  // TL_COLOR_H
