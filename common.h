/*!
 * \file common.h
 * \brief Common functions and macros.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_COMMON_H
#define TL_COMMON_H

//--------------------- Included for easy debugging ---------------
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//-------------------- Macros for safety checks ------------------------
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
 * \brief Die.
 */
#define DIE \
  do { \
    std::cerr << "Killed in " << __FILE__ \
              << " line " << __LINE__ << std::endl; \
    std::exit(EXIT_FAILURE); \
  } while (false)

/*!
 * \brief Die with message.
 */
#define DIE_MSG(message) \
  do { \
    std::cerr << "Killed in " << __FILE__ \
              << " line " << __LINE__ << ": " << message << std::endl; \
    std::exit(EXIT_FAILURE); \
  } while (false)

//---------------------- Macros for debugging -----------------------
/*!
 * \brief Print a warning.
 */
#define WARNING(message) \
  do { \
    std::cerr << "Warning in " << __FILE__ \
              << " line " << __LINE__ << ": " << message << std::endl; \
  } while (false)

/*!
 * \brief Print info.
 */
#define INFO(message) \
  do { \
    std::cout << "[INFO] " << message << std::endl; \
  } while (false)

//------------------------- Other macros ------------------------------
//! \cond DoxygenIgnore
// Trick to allow semi-colon at the end of macros used outside functions.
#define MACRO_END \
  extern void nL3ewN8Too8Um2kCCU1NnIL4Q9z8phYslgWk7W8JrvunxPA8L8whgSyDPL4lJyxEU
//! \endcond

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

/*!
 * \brief Disallow copy and assign.
 */
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(TypeName&) = delete;              \
  void operator=(TypeName) = delete

#endif  // TL_COMMON_H
