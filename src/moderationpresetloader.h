#pragma once

#include <QMap>
#include <QString>

struct PresetData
{
  QString label;
  QString duration;
  QString details;
};

class ModerationPresetLoader
{
public:
  static const QString CONFIGURATION_FILE;

  explicit ModerationPresetLoader();
  virtual ~ModerationPresetLoader();

  void loadPresets();
  void savePresets(QMap<QString, QVector<PresetData>> configuration);
  QVector<PresetData> commandPresets(QString command);
  QStringList commands();

private:
  QMap<QString, QVector<PresetData>> m_preset_data;
};
