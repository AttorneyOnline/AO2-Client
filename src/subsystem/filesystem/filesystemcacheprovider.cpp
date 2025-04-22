#include "filesystemcacheprovider.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

FilesystemCacheProvider::FilesystemCacheProvider(
    int reserve_size)
{
    asset_lookup_cache.reserve(reserve_size);
}


void FilesystemCacheProvider::restoreCache(
    QFile *file)
{
    QScopedPointer<QFile> handle(file);
    QDataStream stream(handle.get());

    stream >> asset_lookup_cache;
    stream >> dir_listing_cache;
    stream >> dir_listing_exist_cache;

    qDebug() << "Restored" << asset_lookup_cache.count() << "items from cache file.";
}


void FilesystemCacheProvider::saveCache(
    QFile *file)
{
    QScopedPointer<QFile> handle(file);
    handle.get()->resize(0);
    QDataStream stream(handle.get());

    stream << asset_lookup_cache;
    stream << dir_listing_cache;
    stream << dir_listing_exist_cache;
}

bool FilesystemCacheProvider::contains(CacheType type, size_t vpath)
{
    // One could tidy this up with a map.
    // I don't think I will
    switch (type) {
    case ASSET:
        return asset_lookup_cache.contains(vpath);
    case DIR_LISTING:
        return dir_listing_cache.contains(vpath);
    case DIR_EXISTS:
        return dir_listing_exist_cache.contains(vpath);
    }
}
