#include "aoutils.h"

#include <QMap>
#include <QSettings>
#include <QDebug>

void AOUtils::migrateEffects(QFile *p_file)
{
  qDebug() << "Migrating effect.in at " << p_file->fileName();
  QMap<QString, QString> l_effects_settings;
  QSettings *l_effects_ini = new QSettings(p_file->fileName(), QSettings::IniFormat);
  l_effects_ini->setIniCodec("UTF-8");
  {
    // Load all known keys into QMap and clear the old ini
    QStringList keys = l_effects_ini->childKeys();
    for (const QString &key : qAsConst(keys)) {
      l_effects_settings.insert(key, l_effects_ini->value(key).toString()); // Don't care about datatype for now.
    }
    l_effects_ini->clear();
    l_effects_ini->sync();
  }
  l_effects_ini->setValue("Version/major", "2"); // This is the second revision of effects.ini

  // All abandon hope who read beyond this line. You have been warned.

  // Due to the annoying nature of lexiographic sorting of QSettings we need an index to order the file.
  int l_effect_sort_index = -1;

  for (auto effectKeyIterator = l_effects_settings.keyBegin(), IteratorEnd = l_effects_settings.keyEnd();
       effectKeyIterator != IteratorEnd; ++effectKeyIterator) {

    // This split size determines how we process the string :
    //  Size 1 : We have a category name that will become sound and be added as a name.
    //  Size 2 : We have a key that needs to be handled, we discard the old group part and take only the key.
    //  Size 3 : We discard the group part and merge the keys back into a two_word key.
    QString l_key = effectKeyIterator.operator*();
    QStringList l_split_key = l_key.split("_");

    if (l_split_key.size() == 1) {
      // We implicity create the group this way too!
      l_effect_sort_index++;
      l_effects_ini->setValue(QString::number(l_effect_sort_index) + "/name", l_key);
      l_effects_ini->setValue(QString::number(l_effect_sort_index) + "/sound", l_effects_settings.value(l_key));
      l_effects_ini->setValue(QString::number(l_effect_sort_index) + "/cull", "true");
    }

    if (l_split_key.size() == 2) {
      l_effects_ini->setValue(QString::number(l_effect_sort_index) + "/" + l_split_key.at(1), l_effects_settings.value(l_key));
    }

    if (l_split_key.size() == 3) {
      QString l_key_name = l_split_key.at(1) + "_" + l_split_key.at(2);
      l_effects_ini->setValue(QString::number(l_effect_sort_index) + "/" + l_key_name, l_effects_settings.value(l_key));
    }

    l_effects_ini->sync();
    }
}
