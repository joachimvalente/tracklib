/*!
 * \file common.h
 * \brief Common functions and macros.
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_COMMON_H
#define TL_COMMON_H

#include <cassert>

#define CHECK(x) assert(x)
#define CHECK_NOTNULL(x) CHECK(x != nullptr)

#endif  // TL_COMMON_H
