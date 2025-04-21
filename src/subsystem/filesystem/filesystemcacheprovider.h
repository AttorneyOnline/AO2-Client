#pragma once

#include <QObject>
#include <QString>
#include <QSet>
#include <QHash>

class QFile;

class FilesystemCacheProvider : public QObject
{
    Q_OBJECT
public:
    FilesystemCacheProvider(QObject* parent) : QObject{parent} {}

    void restoreCache(QFile *file);
    void saveCache(QFile* file);

    enum ErrorCode { BAD_FILE_FORMAT, BAD_FILE_TOO_OLD, BAD_FILE_TOO_NEW };

Q_SIGNALS:
    void errorOccured(ErrorCode error);

private:
    QHash<size_t, QString> asset_lookup_cache;
    QHash<size_t, QString> dir_listing_cache;
    QSet<size_t> dir_listing_exist_cache;
};
