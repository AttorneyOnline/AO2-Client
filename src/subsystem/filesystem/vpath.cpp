#include "vpath.h"

VPath::VPath(
    const QString &str)
    : QString(str)
{}

const QString &VPath::toQString() const
{
    return *this;
}

bool VPath::operator==(
    const VPath &str) const
{
    return this->toQString() == str.toQString();
}

VPath VPath::operator+(
    const VPath &str) const
{
    return VPath(this->toQString() + str.toQString());
}

size_t qHash(
    const VPath &key, uint seed)
{
    return qHash(key.toQString(), seed);
}
