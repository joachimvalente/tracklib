#ifndef MULTITRACK_PARAM_H
#define MULTITRACK_PARAM_H

#include <QDataStream>
#include <QString>

namespace Multitrack {

class Param {
public:
  Param();
  Param(const QString &s);
  Param(float f);
  Param(double d);
  Param(int i);
  Param(bool b);

  void Set(const QString &s);
  void Set(float f);
  void Set(double d);
  void Set(int i);
  void Set(bool b);

  const QString &GetS() const;
  float GetF() const;
  int GetI() const;
  bool GetB() const;


private:
  enum {
    kString,
    kFloat,
    kInt,
    kBool,
  } type_;

  QString s_;
  float f_;
  quint32 i_;
  bool b_;

public:
  friend QDataStream &operator<<(QDataStream &out, const Param &p);
  friend QDataStream &operator>>(QDataStream &in, Param &p);
};

}  // namespace Multitrack

#endif  // MULTITRACK_PARAM_H
