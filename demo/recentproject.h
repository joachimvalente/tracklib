// recentproject.h
//
// Name and path of a recent project.

#ifndef MULTITRACK_RECENTPROJECT_H
#define MULTITRACK_RECENTPROJECT_H

#include <QDataStream>
#include <QString>

namespace Multitrack {

class RecentProject {
public:
  RecentProject();
  RecentProject(const QString &name, const QString &path);

  const QString &name() const;
  const QString &path() const;

  bool operator==(const RecentProject &recent_project);

private:
  QString name_;
  QString path_;

public:
  // Serialization.
  friend QDataStream &operator<<(QDataStream &out, const RecentProject &p);
  friend QDataStream &operator>>(QDataStream &in, RecentProject &p);
};

}  // namespace Multitrack

#endif  // MULTITRACK_RECENTPROJECT_H
