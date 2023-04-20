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
      void populateDirectoryCache(QStringList f_base_mounts);

      QString checkDirectoryCache(VPath f_directory);
      QString checkAssetCache(VPath f_asset);
      void insertIntoDirectoryCache(VPath f_directory, QString f_physical_path);
      void insertIntoAssetCache(VPath f_asset, QString f_physical_path);

      /**
       * @brief Deletes the asset and directory cache.
       *
       * @details In some instances we need to completely redo the cache in order to accept new content, such as :
       *
       * - Adding a mountpath
       * - Removing a mountpath
       * - Changing mountpath order
       * (Generally, if you touch your mountpaths, you will have a bad time.
       *
       * This is necessary to prevent stale entries and incorrect lookups.
       */
      void invalidateCache(QString f_reason);

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
