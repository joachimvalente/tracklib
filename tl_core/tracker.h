/*! \file tracker.h
 * \author Joachim Valente <joachim.valente@gmail.com>
 */

#ifndef TL_TRACKER_H
#define TL_TRACKER_H

namespace tl {

/*! Abstract base class of any tracker.
 * - Initialize with an initial frame, an initial object position/scale and
 *   algorithms params.
 * - Feed new frames online via `NextFrame()`.
 * - Run the tracker via `Process()`.
 * - Get new position/scale via `GetState()` and confidence via
 *   `GetConfidence()`.
 */
class Tracker {
public:
  Tracker();
};

}  // namespace tl

#endif  // TL_TRACKER_H
