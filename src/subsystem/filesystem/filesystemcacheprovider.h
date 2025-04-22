#pragma once

#include <QString>
#include <QSet>
#include <QHash>

class QFile;

/*
 * - Salanto, 2025-04-22, after a night without sleep :
 * This entire class is a bit of a weird one. Technically it would be part of a proper asset pathing class instead of its own class.
 * Unfortunately too much of AOs pathing is in AOApplication to savely extract them to a general Mount/Asset Provider.
 * Best bet currently is to encapsulate this as best I can till I can swap more media to buffer reading.
 *
 *
 * This code should not be considered integral.
 */

class FilesystemCacheProvider
{
public:
    enum CacheType { ASSET, DIR_LISTING, DIR_EXISTS };

    FilesystemCacheProvider(int reserve_size);

    void restoreCache(QFile *file);
    void saveCache(QFile *file);

    bool contains(CacheType type, size_t vpath);
    void insert(CacheType type, size_t vpath, QString phys_path);
    QString value(CacheType type, size_t vpath);

private:
    QHash<size_t, QString> asset_lookup_cache;
    QHash<size_t, QString> dir_listing_cache;
    QSet<size_t> dir_listing_exist_cache;
};
