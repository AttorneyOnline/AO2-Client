#pragma once

#include <QSettings>

namespace AOUtils
{
/**
 * @brief Migrates the effects from the old format to version 2.
 * @param QSettings object reference of the old effects.ini
 */
void migrateEffects(QSettings &p_fileName);
}; // namespace AOUtils
