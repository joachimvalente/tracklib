// param.cpp

#include "param.h"

#include <cassert>

namespace Multitrack {

Param::Param() {}

Param::Param(const QString &s) {
  Set(s);
}

Param::Param(float f) {
  Set(f);
}

Param::Param(double d) {
  Set(d);
}

Param::Param(int i) {
  Set(i);
}

Param::Param(bool b) {
  Set(b);
}

void Param::Set(const QString &s) {
  type_ = Param::kString;
  s_ = s;
}

void Param::Set(float f) {
  type_ = Param::kFloat;
  f_ = f;
}

void Param::Set(double d) {
  Set(static_cast<float>(d));
}

void Param::Set(int i) {
  type_ = Param::kInt;
  i_ = quint32(i);
}

void Param::Set(bool b) {
  type_ = Param::kBool;
  b_ = b;
}

const QString &Param::GetS() const {
  assert(type_ == Param::kString);
  return s_;
}

float Param::GetF() const {
  assert(type_ == Param::kFloat);
  return f_;
}

int Param::GetI() const {
  assert(type_ == Param::kInt);
  return i_;
}

bool Param::GetB() const {
  assert(type_ == Param::kBool);
  return b_;
}

QDataStream &operator<<(QDataStream &out, const Param &p) {
  if (p.type_ == Param::kString) {
    out << 0 << p.s_;
  } else if (p.type_ == Param::kFloat) {
    out << 1 << p.f_;
  } else if (p.type_ == Param::kInt) {
    out << 2 << p.i_;
  } else {
    out << 3 << p.b_;
  }
  return out;
}

QDataStream &operator>>(QDataStream &in, Param &p) {
  int c = 0;
  in >> c;
  if (c == 0) {
    p.type_ = Param::kString;
    in >> p.s_;
  } else if (c == 1) {
    p.type_ = Param::kFloat;
    in >> p.f_;
  } else if (c == 2) {
    p.type_ = Param::kInt;
    in >> p.i_;
  } else {
    p.type_ = Param::kBool;
    in >> p.b_;
  }
  return in;
}

}  // namespace Multitrack
