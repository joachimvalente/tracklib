// project.h
//
// A Multitrack project.

#ifndef MULTITRACK_PROJECT_H
#define MULTITRACK_PROJECT_H

#include <QDataStream>
#include <QString>
#include <QStringList>
#include <QVector>

#include "trackingtask.h"

namespace Multitrack {

class TrackingTask;

class Project {
public:
  enum SourceType { kSourceTypeVideo, kSourceTypeFrames };

  Project();

  const QString &name() const;
  const SourceType &source_type() const;
  const QString &video_path() const;
  int first_frame() const;
  int last_frame() const;
  const QStringList &frame_paths() const;
  float fps() const;

  const QVector<TrackingTask *> &tasks() const;

  void set_name(const QString &name);
  void set_source_type(const SourceType &source_type);
  void set_video_path(const QString &video_path);
  void set_first_frame(int first_frame);
  void set_last_frame(int last_frame);
  void set_frame_paths(const QStringList &frame_paths);
  void set_fps(float fps);

  void add_tracking_task(TrackingTask *task);
  void remove_task(int index);

  void clear_tasks();

  bool IsValid(QString *error) const;

private:
  QString name_;
  SourceType source_type_;

  // If video...
  QString video_path_;
  int first_frame_;
  int last_frame_;

  // If frames...
  QStringList frame_paths_;
  float fps_;

  QVector<TrackingTask *> tasks_;


public:
  // Serialization.
  friend QDataStream &operator<<(QDataStream &out, const Project &p);
  friend QDataStream &operator>>(QDataStream &in, Project &p);
};

}  // namespace Multitrack

#endif  // MULTITRACK_PROJECT_H
