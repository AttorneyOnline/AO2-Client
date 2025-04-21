#include "filesystemcacheprovider.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

Q_GLOBAL_STATIC(QString, CACHE_HEADER, "AO_CACHE_FILE")
const uint CACHE_VERSION = 021102; // 2.11.2



void FilesystemCacheProvider::restoreCache(
    QFile *file)
{
    QDataStream stream(file);
    QString header;
    stream >> header;
    if (header != QString(CACHE_HEADER->data())) {
        qWarning() << "Failed to load cache file. Received header"
            << header << ".Expected Header: " << CACHE_HEADER->data();
    }
}

void FilesystemCacheProvider::saveCache(
    QFile *file)
{
    file->resize(0);
    QDataStream stream(file);
    stream << CACHE_HEADER->data();
    stream << CACHE_VERSION;

    stream.setVersion(QDataStream::Qt_6_5);
}
