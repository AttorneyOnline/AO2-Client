#pragma once

#include <QString>
#include <QHashSeed>

static const int MAGIC_SEED = 0;

class VPath : QString
{
    using QString::QString;

public:
    explicit VPath(const QString &str);

    const QString &toQString() const;
    bool operator==(const VPath &str) const;
    VPath operator+(const VPath &str) const;
};

size_t qHash(const VPath &key, uint seed = MAGIC_SEED);

