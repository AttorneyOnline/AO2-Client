#ifndef OPTIONS_H
#define OPTIONS_H

#include "datatypes.h"

#include <QSettings>


class Options {
private:
  QSettings config;

public:
  Options() : config("config.ini", QSettings::IniFormat) {}

  QString theme() const;

  QString oocName() const;

  int blipRate() const;
  bool blankBlipsEnabled() const;

  int defaultMusicVolume() const;
  int defaultSfxVolume() const;
  int defaultBlipVolume() const;

  bool discordEnabled() const;

  int maxLogLines() const;
  bool legacyScrollEnabled() const;

  QString audioDevice() const;

  bool shownamesEnabled() const;

  QStringList callWords() const;

  void addToFavoriteServers(const server_type &server);
  QVector<server_type> favoriteServers() const;
};

#endif // OPTIONS_H
