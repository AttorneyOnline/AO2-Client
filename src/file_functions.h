#pragma once

#include <QString>

bool file_exists(QString file_path);
bool dir_exists(QString file_path);
bool exists(QString p_path);

QString get_app_path();
QString get_base_path();
