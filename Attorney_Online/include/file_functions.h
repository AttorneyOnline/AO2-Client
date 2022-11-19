#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include <QDir>
#include <QFileInfo>
#include <QString>

bool file_exists(QString file_path);
bool dir_exists(QString file_path);
bool exists(QString p_path);

#endif // FILE_FUNCTIONS_H
