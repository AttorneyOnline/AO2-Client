#pragma once

#include "aoapplication.h"

#include <QSoundEffect>

class AOBlipPlayer
{
public:
  static constexpr int STREAM_COUNT = 5;

  AOBlipPlayer(AOApplication *ao_app);

  void setVolume(int value);
  void setMuted(bool enabled);

  void setBlip(QString blip);

  void playBlip();

private:
  AOApplication *ao_app;

  int m_volume = 0;
  bool m_muted = false;
  QSoundEffect m_stream[STREAM_COUNT];
  int m_cycle = 0;

  void updateInternalVolume();

  // QSoundEffect only plays wav. Decode anything else to a temp wav and
  // cache by source path. Returns the playable path (same as input for wav).
  static QString resolveToPlayablePath(const QString &sourcePath);
};
