#pragma once

#include "bass.h"
#include "bassopus.h"

#include "aoapplication.h"

#include <QDebug>
#include <QWidget>

class AOSfxPlayer
{
public:
  static constexpr int CHANNEL_COUNT = 5;

  AOSfxPlayer(AOApplication *p_ao_app);

  int get_volume();

  void clear();
  void loop_clear();
  void play(QString p_sfx, QString p_char = QString(), QString shout = QString());
  void stop(int channel = -1);
  void set_volume(qreal p_volume);
  void set_looping(bool toggle, int channel = -1);
  void set_muted(bool toggle);

private:
  AOApplication *ao_app;

  qreal m_volume = 0.0;
  bool m_looping = true;
  bool m_muted = false;
  int m_channel = 0;
  HSTREAM m_stream_list[CHANNEL_COUNT]{};

  void set_volume_internal(qreal p_volume);
};
