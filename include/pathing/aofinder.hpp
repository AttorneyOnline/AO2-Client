#ifndef AOFINDER_H
#define AOFINDER_H

#include <QObject>
#include <memory>

namespace AttorneyOnline::Core {
  class AOCache;
}

class AOFinder : public QObject {
public:
  AOFinder(QObject *parent, QString f_base_path);
  ~AOFinder();

private:
  std::unique_ptr<AttorneyOnline::Core::AOCache> m_cache;

public slots:
  void updateMountPaths(QStringList f_base_mounts, QString f_reason);
};

#endif // AOFINDER_H
