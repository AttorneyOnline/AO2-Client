#pragma once

#include <QHash>
#include <QString>

class VPath : QString
{
  using QString::QString;

public:
  explicit VPath(const QString &str);

  const QString &toString() const;

  bool operator==(const VPath &str) const;
  VPath operator+(const VPath &str) const;
};

size_t qHash(const VPath &key, size_t seed = 0);
