// videoplayer.h
//
// Play videos.

#ifndef MULTITRACK_VIDEOPLAYER_H
#define MULTITRACK_VIDEOPLAYER_H

#include <QObject>
#include <QString>
#include <QTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "abstractplayer.h"

namespace Multitrack {

class VideoPlayer : public AbstractPlayer {
  Q_OBJECT

public:
  explicit VideoPlayer(const QString &filename,
                       int first_frame, int last_frame);

public slots:
  void Initialize();
  void Previous();  // Display previous frame.
  void First();  // Display first frame.
  void Next();  // Display next frame.
  void GotoFrame(int frame);  // Go to specified frame.

private:
  cv::VideoCapture cap_;
  const QString &filename_;
  const int first_frame_;
  const int last_frame_;
  QTime timer_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_VIDEOPLAYER_H
