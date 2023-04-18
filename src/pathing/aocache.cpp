#include "pathing/aocache.hpp"
#include "options.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>

AttorneyOnline::Core::AOCache::AOCache(QObject *parent, const QString f_base_path)
    : QObject(parent), m_base_path(f_base_path)
{
  qInfo() << "[Cache]::CTOR : Starting";

  if (!Options::getInstance().enablePersistentCache()) {
    qInfo() << "[Cache]::CTOR : Cache restore skipped.";
    return;
  }

  QString l_directory_path = f_base_path + "/dir_cache.ao";
  if (QFile::exists(l_directory_path)) {
    qInfo() << "[Cache]::CTOR : Restoring directory cache.";

    QFile l_directory_file(l_directory_path);
    if (l_directory_file.open(QIODevice::ReadOnly)) {
      QDataStream l_read(&l_directory_file);
      l_read >> m_directory_cache;
    }
  }

  QString l_asset_path = f_base_path + "/asset_cache.ao";
  if (QFile::exists(l_directory_path)) {
    qInfo() << "[Cache]::CTOR : Restoring asset cache.";

    QFile l_asset_file(l_asset_path);
    if (l_asset_file.open(QIODevice::ReadOnly)) {
      QDataStream l_read(&l_asset_file);
      l_read >> m_directory_cache;
    }
  }
}

AttorneyOnline::Core::AOCache::~AOCache()
{
  if (!Options::getInstance().enablePersistentCache()) {
    QFile::remove(m_base_path + "/dir_cache.ao");
    QFile::remove(m_base_path + "/asset_cache.ao");
    return;
  }

  QString l_directory_path = m_base_path + "/dir_cache.ao";
  qInfo() << "[Cache]::DTOR : Saving directory cache.";

  QFile l_directory_file(l_directory_path);
  l_directory_file.resize(0);
  if (l_directory_file.open(QIODevice::WriteOnly)) {
    QDataStream l_out(&l_directory_file);
    l_out << m_directory_cache;
    l_directory_file.close();
  }

  QString l_asset_path = m_base_path + "/asset_cache.ao";
  qInfo() << "[Cache]::DTOR : Saving asset cache.";

  QFile l_asset_file(l_asset_path);
  l_asset_file.resize(0);
  if (l_asset_file.open(QIODevice::WriteOnly)) {
    QDataStream l_out(&l_asset_file);
    l_out << m_directory_cache;
    l_asset_file.close();
  }
}
