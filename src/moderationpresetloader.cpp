#include "moderationpresetloader.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const QString ModerationPresetLoader::CONFIGURATION_FILE = "user/moderation_presets.json";

ModerationPresetLoader::ModerationPresetLoader()
{
  loadPresets();
  savePresets(m_preset_data);
}

ModerationPresetLoader::~ModerationPresetLoader()
{}

void ModerationPresetLoader::loadPresets()
{
  m_preset_data.clear();
  QFile config(CONFIGURATION_FILE);
  if (!config.open(QIODevice::ReadOnly))
  {
    qWarning() << "Unable to open file" << CONFIGURATION_FILE << config.errorString();
    return;
  }

  QJsonParseError error;
  QJsonDocument document = QJsonDocument::fromJson(config.readAll(), &error);
  if (error.error != QJsonParseError::NoError)
  {
    qDebug() << "Malformed JSON. Error" << error.errorString();
    return;
  }

  const QJsonArray commands = document.array();
  for (int entry = 0; entry < commands.size(); entry++)
  {
    const QJsonObject command = commands[entry].toObject();
    const QJsonArray presets = command["presets"].toArray();

    QVector<PresetData> list;
    for (int p_entry = 0; p_entry < presets.size(); p_entry++)
    {
      const QJsonObject data = presets[p_entry].toObject();
      PresetData preset;
      preset.label = data["label"].toString();
      preset.duration = data["duration"].toString();
      preset.details = data["details"].toString();
      list.append(preset);
    }
    m_preset_data.insert(command["name"].toString(), list);
  }
}

void ModerationPresetLoader::savePresets(QMap<QString, QVector<PresetData>> configuration)
{
  QFile config(CONFIGURATION_FILE);
  if (!config.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    qWarning() << "Unable to open file" << CONFIGURATION_FILE << config.errorString();
    return;
  }

  QJsonArray commands;
  const QStringList names = configuration.keys();
  for (const QString &name : names)
  {
    QVector<PresetData> presets = configuration.value(name);
    QJsonArray list;
    for (const PresetData &data : presets)
    {
      QJsonObject preset;
      preset["label"] = data.label;
      preset["duration"] = data.duration;
      preset["details"] = data.details;

      list.append(preset);
    }
    QJsonObject command;
    command["name"] = name;
    command["presets"] = list;
    commands.append(command);
  }
  config.write(QJsonDocument(commands).toJson(QJsonDocument::Indented));
}

QVector<PresetData> ModerationPresetLoader::commandPresets(QString command)
{
  return m_preset_data.value(command, QVector<PresetData>());
}

QStringList ModerationPresetLoader::commands()
{
  return m_preset_data.keys();
}
