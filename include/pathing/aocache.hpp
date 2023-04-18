#ifndef AOCACHE_H
#define AOCACHE_H
#pragma once

#include <QHash>
#include <QObject>

#include "pathing/aovpath.hpp"

namespace AttorneyOnline {
  namespace Core {
    class AOCache : public QObject {
    public:
      explicit AOCache(QObject* parent, const QString f_base_path);
      ~AOCache();

    private:
      QHash<VPath, QString> m_asset_cache;
      QHash<VPath, QString> m_directory_cache;

      QString m_base_path;
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // AOCACHE_H
