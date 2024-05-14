#pragma once

#include "bass.h"
#include "bassopus.h"

#include "aoapplication.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QWidget>
#include <string.h>

class AOBlipPlayer
{
public:
  AOBlipPlayer(AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);
  void set_muted(bool toggle);

private:
  AOApplication *ao_app;
  qreal m_volume = 0.0;
  int m_cycle = 0;
  bool m_muted = false;
  HSTREAM m_stream_list[5];

  void set_volume_internal(qreal p_volume);
};
