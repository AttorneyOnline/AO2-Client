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

      /**
       * @brief Populates the directory cache for the root asset directories
       *
       * @details Checks all available mount paths for asset folders.
       * Will index the root folder paths, i.E :
       *  - characters/Phoenix
       *  - characters/Apollo
       *  - backgrounds/PLvsPWWitchCourt
       *
       *  Note : This means assets in the same subfolder can no longer
       *  be split in multiple mount paths.
       */
      void populateDirectoryCache();

      QString checkDirectoryCache(VPath f_directory);
      QString checkAssetCache(VPath f_asset);

    private:
      //uint = hashed VPath
      QHash<uint, QString> m_asset_cache;
      QHash<uint, QString> m_directory_cache;

      QString m_base_path;

      /**
       * @brief Wether the client is the first to launch and delete the lock file on close.
       */
      bool m_primary_client = false;

      bool isPrimaryClient();
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // AOCACHE_H
