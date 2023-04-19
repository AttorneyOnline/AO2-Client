#ifndef AOVPATH_H
#define AOVPATH_H

#include <QString>
#include <QHash>
namespace AttorneyOnline {
  namespace Core {
    class VPath : QString {
      using QString::QString;

    public:
      explicit VPath(const QString &str) : QString(str) {}
      inline QString const &toQString() const { return *this; }
      inline bool operator==(const VPath &str) const {
        return this->toQString() == str.toQString();
      }
      inline VPath operator+(const VPath &str) const {
        return VPath(this->toQString() + str.toQString());
      }
    };

    inline uint qHash(const VPath &key, uint seed = qGlobalQHashSeed())
    {
      return qHash(key.toQString(), seed);
    }
  }
}

using VPath = AttorneyOnline::Core::VPath;

#endif // AOVIEWPATH_H
