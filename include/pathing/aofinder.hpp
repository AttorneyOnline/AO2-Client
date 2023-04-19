#ifndef AOFINDER_H
#define AOFINDER_H

#include <QObject>
#include <memory>

#include "pathing/aovpath.hpp"

namespace AttorneyOnline::Core {
  class AOCache;
}

class AOFinder : public QObject {
  AOFinder(QObject *parent, QString f_base_path);
  ~AOFinder();

private:

  std::unique_ptr<AttorneyOnline::Core::AOCache> m_cache;
};

#endif // AOFINDER_H
