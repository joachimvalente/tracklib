/*!
 * \file common.h
 * \brief Common functions and macros.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_COMMON_H
#define TL_COMMON_H

#include <iostream>

//! \cond DoxygenIgnore
// Trick to allow semi-colon at the end of macros used outside functions.
#define MACRO_END \
  extern void nL3ewN8Too8Um2kCCU1NnIL4Q9z8phYslgWk7W8JrvunxPA8L8whgSyDPL4lJyxEU
//! \endcond

/*!
 * \brief Die if `condition` is not verified.
 */
#define CHECK(condition) \
  do { \
    if (!(condition)) { \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                << " line " << __LINE__ << std::endl; \
      std::exit(EXIT_FAILURE); \
    } \
  } while (false)

/*!
 * \brief Die if `x` is nullptr.
 */
#define CHECK_NOTNULL(x) CHECK(x != nullptr)

/*!
 * \brief Die with `message` if `condition` is not verified.
 */
#define CHECK_MSG(condition, message) \
  do { \
    if (!(condition)) { \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                << " line " << __LINE__ << ": " << message << std::endl; \
      std::exit(EXIT_FAILURE); \
    } \
  } while (false)

/*!
 * \brief Use enum as bit flags.
 */
#define BITFLAGS(classname) \
  ; \
  inline classname operator|(classname a, classname b) \
  { \
    return static_cast<classname>(static_cast<int>(a) | static_cast<int>(b)); \
  } \
  MACRO_END

#endif  // TL_COMMON_H
