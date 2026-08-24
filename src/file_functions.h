#pragma once

#include <QString>

bool file_exists(QString file_path);
bool dir_exists(QString file_path);
bool exists(QString p_path);

QString get_app_path();
QString get_base_path();

// macOS only: on first run, seed the writable base (in Application Support) from
// the read-only default base bundled in the .app. No-op on other platforms.
void seed_base_if_missing();
