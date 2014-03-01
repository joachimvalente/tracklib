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

//------------------------- Core ------------------------
#include "tl_core/detector.h"
#include "tl_core/filter.h"
#include "tl_core/tracker.h"

//----------------------- Detectors ---------------------
#include "tl_detectors/meanshiftdetector.h"
#include "tl_detectors/nodetector.h"
#include "tl_detectors/templatematchingdetector.h"

//------------------------ Filters ----------------------
#include "tl_filters/kalmanfilter.h"

//-------------------------- Gpu ------------------------
#ifdef TL_CUDA
# include "tl_gpu/templatematchingdetectorgpu.h"
#endif  // ifdef TL_CUDA

#endif  // TRACKLIB_TRACKLIB_H
