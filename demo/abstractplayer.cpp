// abstractplayer.cpp

#include "abstractplayer.h"

#include <QCoreApplication>
#include <QTime>

#include <opencv2/imgproc/imgproc.hpp>

namespace Multitrack {

AbstractPlayer::AbstractPlayer(QObject *parent) :
  QObject(parent),
  nb_frames_(1), current_frame_(1), fps_(25.0f), speed_(1.0f),
  playing_(false), was_playing_(false) {}

float AbstractPlayer::fps() const {
  return fps_;
}

void AbstractPlayer::ProcessFrame(const cv::Mat &frame, const QTime &time) {
  QImage *img;
  if (frame.channels() == 3) {
    img = new QImage(MatColor2QImage(frame));
  } else {
    img = new QImage(MatGray2QImage(frame));
  }
  emit FrameProcessed(img, current_frame_, time);
}

void AbstractPlayer::TogglePlayPause() {
  if (playing_) {
    Pause();
  } else {
    Play();
  }
}

void AbstractPlayer::ChangeSpeed(float speed) {
  speed_ = speed;
}

void AbstractPlayer::Play() {
  playing_ = true;
  emit StartedPlaying();
  while (playing_) {
    QTime t;
    t.start();

    Next();

    while(t.elapsed() < static_cast<int>(1000.0 / (fps_ * speed_)))
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

void AbstractPlayer::Pause() {
  playing_ = false;
  emit Paused();
}

void AbstractPlayer::PauseAlt() {
  was_playing_ = playing_;
  Pause();
}

void AbstractPlayer::PlayAlt() {
  if (was_playing_) Play();
}

QImage AbstractPlayer::MatColor2QImage(const cv::Mat3b &src) {
  QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
  for (int y = 0; y < src.rows; ++y) {
    const cv::Vec3b *srcrow = src[y];
    QRgb *destrow = (QRgb*)dest.scanLine(y);
    for (int x = 0; x < src.cols; ++x) {
      destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
    }
  }
  return dest;
}

QImage AbstractPlayer::MatGray2QImage(const cv::Mat_<double> &src) {
  double scale = 255.0;
  QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
  for (int y = 0; y < src.rows; ++y) {
    const double *srcrow = src[y];
    QRgb *destrow = (QRgb*)dest.scanLine(y);
    for (int x = 0; x < src.cols; ++x) {
      unsigned int color = srcrow[x] * scale;
      destrow[x] = qRgba(color, color, color, 255);
    }
  }
  return dest;
}

}  // namespace Multitrack
