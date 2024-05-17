#pragma once

#include "aoapplication.h"

#include <bass.h>
#include <bassopus.h>

#include <QDebug>
#include <QElapsedTimer>
#include <QWidget>

#include <string.h>

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
  HSTREAM m_stream[STREAM_COUNT]{};
  int m_cycle = 0;

  void updateInternalVolume();
};
