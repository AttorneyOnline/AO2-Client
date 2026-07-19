#pragma once

#include <QSettings>
#include <QString>

namespace AOUtils
{
/**
 * @brief Migrates the effects from the old format to version 2.
 * @param QSettings object reference of the old effects.ini
 */
void migrateEffects(QSettings &p_fileName);

/**
 * @brief Converts plain text to HTML and turns any URLs into links
 */
QString convert_to_html(const QString &p_text);
}; // namespace AOUtils
