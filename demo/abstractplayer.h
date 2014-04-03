// abstractplayer.h
//
// Abstract player that can play either a video or a frame sequence.

#ifndef MULTITRACK_ABSTRACTPLAYER_H
#define MULTITRACK_ABSTRACTPLAYER_H

#include <QImage>
#include <QObject>
#include <QTime>

#include <opencv2/core/core.hpp>

#include "tracklib.h"

namespace Multitrack {

class AbstractPlayer : public QObject {
  Q_OBJECT

public:
  explicit AbstractPlayer(QObject *parent = 0);

  float fps() const;

  static QImage MatColor2QImage(const cv::Mat3b &src);
  static QImage MatGray2QImage(const cv::Mat_<double> &src);

signals:
  void StartedPlaying();
  void Paused();
    // time is the time of the request.
  void FrameProcessed(QImage *img, int frame, const QTime &time);
  void CriticalError(const QString &error);

public slots:
  virtual void Initialize() = 0;
  virtual void Previous() = 0;  // Display previous frame.
  virtual void First() = 0;  // Display first frame.
  virtual void Next() = 0;  // Display next frame.
  virtual void TogglePlayPause();
  virtual void GotoFrame(int frame) = 0;  // Go to specified frame.
  virtual void ChangeSpeed(float speed);  // Change playback speed.
  virtual void Pause();  // Pause.
  virtual void PauseAlt();  // Pause and remember if playing was playing or not.
  virtual void PlayAlt();  // Resume if player was playing.

protected:
  virtual void Play();  // Play movie or sequence.
  void ProcessFrame(const cv::Mat &frame, const QTime &time);

  int nb_frames_;
  int current_frame_;  // Always 1-based.
  float fps_;
  float speed_;
  bool playing_;
  bool was_playing_;

  bool show_foreground_;
//  IBGS *bgs_;
};

}  // namespace Multitrack

#endif  // MULTITRACK_ABSTRACTPLAYER_H
