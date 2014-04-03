// recentproject.cpp

#include "recentproject.h"

namespace Multitrack {

RecentProject::RecentProject() {}

RecentProject::RecentProject(const QString &name, const QString &path) :
  name_(name), path_(path) {}

const QString &RecentProject::name() const {
  return name_;
}

const QString &RecentProject::path() const {
  return path_;
}

bool RecentProject::operator==(const RecentProject &recent_project) {
  return path_ == recent_project.path_;
}

QDataStream &operator<<(QDataStream &out, const RecentProject &p) {
  out << p.name_ << p.path_;
  return out;
}

QDataStream &operator>>(QDataStream &in, RecentProject &p) {
  in >> p.name_ >> p.path_;
  return in;
}

}  // namespace Multitrack
