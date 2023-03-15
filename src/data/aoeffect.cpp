#include "include/data/aoeffect.hpp"

#include <QDebug>
#include <QFile>

AOEffect::AOEffect(const QString effect_ini_path)
    : m_effect_ini(effect_ini_path, QSettings::IniFormat)
{
  qDebug() << "Trying to load effect.ini at" << effect_ini_path;
  if (m_effect_ini.status() != QSettings::NoError) {
    qDebug() << "Error encountered:" << m_effect_ini.status();
  }
}

QString AOEffect::name()
{
  return m_effect_ini.value("config/name", "NoName").toString();
}

QString AOEffect::sound()
{
  return m_effect_ini.value("config/sound", "").toString();
}

bool AOEffect::cull()
{
  return m_effect_ini.value("config/cull", true).toBool();
}

DataTypesAnimation::EffectLayer AOEffect::layer()
{
  return DataTypesAnimation().toDataType<DataTypesAnimation::EffectLayer>(
      m_effect_ini.value("config/layer", "behind").toString().toUpper());
}

bool AOEffect::loop()
{
  return m_effect_ini.value("config/loop", false).toBool();
}

int AOEffect::maxDuration()
{
  return m_effect_ini.value("config/max_duration", 60).toInt();
}

bool AOEffect::applyFlip()
{
  return m_effect_ini.value("config/respect_flip", false).toBool();
}

bool AOEffect::applyOffset()
{
  return m_effect_ini.value("config/respect_offset", false).toBool();
}

Qt::TransformationMode AOEffect::scaling()
{
  QString scaling = m_effect_ini.value("config/foo", "smooth").toString();
  if (scaling == "smooth") {
    return Qt::TransformationMode::SmoothTransformation;
  }
  else {
    return Qt::TransformationMode::FastTransformation;
  }
}

bool AOEffect::remainSelected()
{
  return m_effect_ini.value("config/sticky", false).toBool();
}

bool AOEffect::stretchToViewport()
{
  return m_effect_ini.value("config/stretch", true).toBool();
}
