#ifndef OPTIONS_H
#define OPTIONS_H

#include "datatypes.h"

#include <QSettings>
#include <QCoreApplication>


class Options {
private:
  QSettings config;

  void migrateCallWords();
public:
  Options() : config(
      QCoreApplication::applicationDirPath() + "/base/config.ini",
      QSettings::IniFormat) {}
  void migrate();

  QString theme() const;
  void setTheme(QString value);

  QString oocName() const;
  void setOOCName(QString value);

  int blipRate() const;
  void setBlipRate(int value);

  bool blankBlipsEnabled() const;
  void setBlankBlipsEnabled(bool value);

  int defaultMusicVolume() const;
  void setDefaultMusicVolume(int value);

  int defaultSfxVolume() const;
  void setDefaultSfxVolume(int value);
  int defaultBlipVolume() const;
  void setDefaultBlipVolume(int value);

  bool discordEnabled() const;
  void setDiscordEnabled(bool value);

  int maxLogLines() const;
  void setMaxLogLines(int value);
  bool legacyScrollEnabled() const;
  void setLegacyScrollEnabled(bool value);

  QString audioDevice() const;
  void setAudioDevice(QString value);

  QStringList callWords() const;
  void setCallWords(QStringList value);

  void addToFavoriteServers(const server_type &server);
  QVector<server_type> favoriteServers() const;

  QString msAddress() const;
  void setMSAddress(QString value);
  uint16_t msPort() const;
  void setMSPort(uint16_t value);

  QString language() const;
  void setLanguage(QString value);

  int blipInterval() const;
  void setBlipInterval(int value);

  bool disableLoopingSfx() const;
  void setDisableLoopingSfx(bool value);

  bool objectionStopsMusic() const;
  void setObjectionStopsMusic(bool value);

  bool logNewlineAfterName() const;
  void setLogNewlineAfterName(bool value);

  int logSpacing() const;
  void setLogSpacing(int value);

  bool logTimestamps() const;
  void setLogTimestamps(bool value);

  bool saveLogs() const;
  void setSaveLogs(bool value);

  QSize viewportScale() const;
  void setViewportScale(QSize value);

  bool disableScreenshake() const;
  void setDisableScreenshake(bool value);

  bool disableOverlayEffects() const;
  void setDisableOverlayEffects(bool value);

  bool disableFrameSfx() const;
  void setDisableFrameSfx(bool value);

  bool disableLogColors() const;
  void setDisableLogColors(bool value);

  bool disableCustomChatboxes() const;
  void setDisableCustomChatboxes(bool value);

  bool disableCustomShownames() const;
  void setDisableCustomShownames(bool value);

  bool stickyPreanim() const;
  void setStickyPreanim(bool value);

  bool stickySfx() const;
  void setStickySfx(bool value);

  bool stickyEffects() const;
  void setStickyEffects(bool value);

};

#endif // OPTIONS_H
