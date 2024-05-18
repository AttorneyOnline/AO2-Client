#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>

bool file_exists(QString file_path);
bool dir_exists(QString file_path);
bool exists(QString p_path);
QString get_base_path();
