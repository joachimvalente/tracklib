// trackingtask.h
//
// A tracking task consists of:
//  - a tracker (i.e. a tracking algorithm with specified parameters),
//  - a object to track (and the frame number where it is defined),
//  - the result (objects in subsequent frames).

#ifndef MULTITRACK_TRACKINGTASK_H
#define MULTITRACK_TRACKINGTASK_H

#include <QColor>
#include <QDataStream>
#include <QImage>
#include <QObject>
#include <QRect>
#include <QString>
#include <QStringList>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "param.h"
#include "project.h"

namespace Multitrack {

class Project;

class TrackingTask : public QObject {
  Q_OBJECT

public:
  enum Algorithm {
    kDummyTracker = 0,
    kTemplateMatching,
    kMeanshift,
    kOnlineBoosting,
    kMiltrack,
    kKalman
  };

  enum BGS {
    kBgsNone = 0,
    kBgs
  };

  TrackingTask();
  TrackingTask(const TrackingTask *task);

  Algorithm algorithm() const;
  QString algorithm_str() const;
  const Param &param(int i) const;
  BGS bgs() const;
  int bgs_erode() const;
  int bgs_dilate() const;

  void set_tracker(Algorithm algo, const QVector<Param> &params, BGS bgs,
                   int bgs_erode, int bgs_dilate);
  void set_object(const cv::Rect &object);
  void set_first_frame(int first_frame);

  void set_project_details(bool is_video,
                           const QString &video_path,
                           const QStringList &frame_paths);

  void set_random_color();

  QRect result(int frame) const;
  const QVector<QRect> &results() const;
  bool completed() const;
  bool active() const;
  int first_frame() const;
  const QColor &color() const;
  const QRect &object() const;

  const QString &error() const;
  const cv::Mat &preview() const;

  void ClearResults();

signals:
  void Processed(int frame);
  void Finished();
  void Failed();
  void Preview();  // Preview ready.

public slots:
  // Run the task.
  void Run();

  void set_active(bool active);
  void set_color(const QColor &color);

private:
  // Algorithm.
  Algorithm algo_;
  QVector<Param> params_;

  // Background subtraction.
  BGS bgs_;
  int bgs_erode_;
  int bgs_dilate_;

  // Tracking problem.
  QRect object_;
  int first_frame_;

  // Results.
  QVector<QRect> results_;  // Includes the object itself.
  bool completed_;

  // Details of the project.
  bool is_video_;
  QString video_path_;
  QStringList frame_paths_;

  // UI properties.
  bool active_;  // Whether we visualize it or not.
  QColor color_;
  cv::Mat preview_;

  // Error message.
  QString error_;

  // Conversion functions.
  static QRect CvRect2QRect(const cv::Rect &r);
  static cv::Rect QRect2CvRect(const QRect &q);

  // Random integer from low to high (included).
  static int rand_int(int low, int high);

public:
  // Serialization.
  friend QDataStream &operator<<(QDataStream &out, const TrackingTask *t);
  friend QDataStream &operator>>(QDataStream &in, TrackingTask *t);
};

}  // namespace Multitrack

#endif  // MULTITRACK_TRACKINGTASK_H
