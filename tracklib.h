/*!
 * \file tracklib.h
 * \brief Main file of Tracklib.
 *
 * This file includes all necessary files and should be the only one included
 * in projects.
 * \author Joachim Valente <joachim.valente@gmail.com>
 *
 * \mainpage
 * **Tracklib** is a C++ online object tracking library.
 */

#ifndef TRACKLIB_TRACKLIB_H
#define TRACKLIB_TRACKLIB_H

//----------------------- Detectors --------------------
#include "tl_detectors/dummydetector.h"

//-------------------------- Gpu ------------------------
#ifdef TL_CUDA
# include "tl_gpu/templatematchingdetectorgpu.h"
#endif  // ifdef TL_CUDA

#endif  // TRACKLIB_TRACKLIB_H
