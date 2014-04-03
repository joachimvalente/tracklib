// frameplayer.h
//
// Play frame sequences.

#ifndef MULTITRACK_FRAMEPLAYER_H
#define MULTITRACK_FRAMEPLAYER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "abstractplayer.h"

namespace Multitrack {

class FramePlayer : public AbstractPlayer {
  Q_OBJECT

public:
  explicit FramePlayer(const QStringList &frame_paths, float fps);

public slots:
  void Initialize();
  void Previous();  // Display previous frame.
  void First();  // Display first frame.
  void Next();  // Display next frame.
  void GotoFrame(int frame);  // Go to specified frame.

private:
  const QStringList &frame_paths_;
  QTime timer_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_FRAMEPLAYER_H
