// abstractplayer.cpp

#include "abstractplayer.h"

#include <QCoreApplication>
#include <QTime>

#include <opencv2/imgproc/imgproc.hpp>

//#include "tracklib/extern/SLICSuperpixels/SLIC.h"

namespace Multitrack {

AbstractPlayer::AbstractPlayer(QObject *parent) :
  QObject(parent),
  nb_frames_(1), current_frame_(1), fps_(25.0f), speed_(1.0f),
  playing_(false), was_playing_(false) {

  show_foreground_ = false;
//  bgs_ = new DPAdaptiveMedianBGS;
}

float AbstractPlayer::fps() const {
  return fps_;
}

void AbstractPlayer::ProcessFrame(const cv::Mat &frame, const QTime &time) {
  QImage *img;
  if (frame.channels() == 3) {
//    unsigned int *image = new unsigned int[frame.rows * frame.cols * 3];
//    for (int i = 0; i < frame.rows; ++i) {
//      for (int j = 0; j < frame.cols; ++j) {
//        for (int k = 0; k < 3; ++k) {
//          image[k * (frame.rows * frame.cols) + i + frame.rows * j] =
//              frame.data[k + i * 3 + j * 3 * frame.rows];
//        }
//      }
//    }

//    SLIC slic;
//    int *klabels = new int[frame.rows * frame.cols];
//    int numlabels = 0;
//    int k = 2000;  // Number of superpixels.
//    double m = 20;  // Compression factor.
//    slic.PerformSLICO_ForGivenK(image,
//                                frame.cols, frame.rows, klabels,
//                                numlabels, k, m);

//    double *superpixel_values = new double[3 * numlabels];
//    for (int i = 0; i < 3 * numlabels; ++i) superpixel_values[i] = 0;

//    int *nb_pixels = new int[numlabels];
//    for (int i = 0; i < numlabels; ++i) nb_pixels[i] = 0;

//    for (int i = 0; i < frame.rows; ++i) {
//      for (int j = 0; j < frame.cols; ++j) {
//        for (int k = 0; k < 3; ++k) {
//          superpixel_values[k * numlabels + klabels[j + frame.cols * i]] +=
//              frame.at<cv::Vec3b>(i, j)[k];
//        }
//        ++nb_pixels[klabels[j + frame.cols * i]];
//      }
//    }
//    for (int i = 0; i < numlabels; ++i) {
//      for (int k = 0; k < 3; ++k) {
//        superpixel_values[k * numlabels + i] /= nb_pixels[i];
//      }
//    }

//    cv::Mat_<cv::Vec3b> frame2 = frame;
//    for (int i = 0; i < frame.rows; ++i) {
//      for (int j = 0; j < frame.cols; ++j) {
//        for (int k = 0; k < 3; ++k) {
//          frame2(i, j)[k] = superpixel_values[k * numlabels + klabels[j + frame.cols * i]];
//        }
//      }
//    }

    if (show_foreground_) {
//      cv::Mat bg, fg, masked;
//      bgs_->process(frame, fg /* foreground */, bg /* background */);
//      cv::erode(fg, fg, Mat(), Point(-1, -1), 1);
//      cv::dilate(fg, fg, Mat(), Point(-1, -1), 25);
//      frame.copyTo(masked, fg);
//      img = new QImage(MatColor2QImage(masked));
    } else {
      img = new QImage(MatColor2QImage(frame));
    }
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
