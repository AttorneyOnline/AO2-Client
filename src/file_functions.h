#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QCoreApplication>

bool file_exists(QString file_path);
bool dir_exists(QString file_path);
bool exists(QString p_path);
QString get_app_dir_path();
QString get_base_path();
QString get_packages_path();
bool copyDirectoryRecursive(const QString &sourcePath, const QString &destPath);
bool moveDirectoryInto(const QString &sourcePath, const QString &destPath, QString *error);
bool moveFileInto(const QString &sourcePath, const QString &destPath, QString *error);
bool moveDirectoryContentsInto(const QString &sourcePath,
                               const QString &destPath, QString *error,
                               bool overwrite = false);

// macOS only: on first run, copy the default base bundled in the .app into the
// writable Application Support location. No-op on other platforms.
void seed_base_if_missing();

#endif // FILE_FUNCTIONS_H
