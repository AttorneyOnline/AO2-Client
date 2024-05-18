#include "aoutils.h"

#include <QDebug>
#include <QMap>
#include <QPair>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>

void AOUtils::migrateEffects(QSettings &p_effects_ini)
{
  qDebug() << "Migrating effects from file:" << p_effects_ini.fileName();

  QMap<QString, QString> l_effect_map;
  {
    const QStringList l_key_list = p_effects_ini.childKeys();
    for (const QString &i_key : l_key_list)
    {
      l_effect_map.insert(i_key, p_effects_ini.value(i_key).toString());
    }

    p_effects_ini.clear();
    p_effects_ini.sync();
  }
  // update revision
  p_effects_ini.beginGroup("version");
  p_effects_ini.setValue("major", "2");
  p_effects_ini.endGroup();

  const QStringList l_property_list{
      "sound",
      "scaling",
      "stretch",
      "ignore_offset",
      "under_chatbox",
  };

  const QMap<QString, QPair<QString, QString>> l_property_replacement_list{
      {"under_chatbox", {"layer", "character"}},
  };

  QStringList l_key_list;
  // Matches old effects.ini childKeys that are not the effect name, but a property that applies to an effect.
  // This is due to the name also being used as a key to apply the sfx property.
  // Example :
  // realization_scaling - This would not be appended to the key_list as it matches scaling property.
  // realization_alt - This would be appened as it contains an underscore, but not a property.
  // hearts - This would be appended as it does not contain a property
  const QRegularExpression l_regex(QStringLiteral("(\\w+)_(%1)$").arg(l_property_list.join("|")));
  for (auto i = l_effect_map.begin(); i != l_effect_map.end(); i++)
  {
    if (l_regex.match(i.key()).hasMatch())
    {
      continue;
    }
    l_key_list.append(i.key());
  }

  int i = 0;
  for (const QString &i_effect_key : qAsConst(l_key_list))
  {
    p_effects_ini.beginGroup(QString::number(i++));
    p_effects_ini.setValue("name", i_effect_key);
    p_effects_ini.setValue("sound", l_effect_map.value(i_effect_key));
    p_effects_ini.setValue("cull", true);
    p_effects_ini.setValue("layer", "character");

    if (i_effect_key == "realization")
    {
      p_effects_ini.setValue("stretch", true);
      p_effects_ini.setValue("layer", "chat");
    }

    for (const QString &i_property : l_property_list)
    {
      const QString l_property_key = QString("%1_%2").arg(i_effect_key, i_property);
      if (l_effect_map.contains(l_property_key))
      {
        const QString l_property_value = l_effect_map.value(l_property_key);
        const auto [l_converted_key, l_converted_value] = l_property_replacement_list.value(i_property, {i_property, l_property_value});
        p_effects_ini.setValue(l_converted_key, l_converted_value);
      }
    }

    p_effects_ini.endGroup();
  }
  p_effects_ini.sync();
}
