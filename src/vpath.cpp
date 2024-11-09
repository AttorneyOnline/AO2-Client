#include "vpath.h"



VPath::VPath(const QString &str)
  : QString(str)
{}

const QString &VPath::toString() const { return *this; }

bool VPath::operator==(const VPath &str) const { return this->toString() == str.toString(); }

VPath VPath::operator+(const VPath &str) const { return VPath(this->toString() + str.toString()); }

size_t qHash(const VPath &key, size_t seed)
{
  return qHash(key.toString(), seed);
}
