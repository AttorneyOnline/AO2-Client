#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"
#include "bassopus.h"

#include "aoapplication.h"

#include <QDebug>
#include <QWidget>
#include <string.h>

class AOSfxPlayer {
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void clear();
  void loop_clear();
  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop(int channel = -1);
  void set_volume(qreal p_volume);
  void set_looping(bool toggle, int channel = -1);
  int m_channel = 0;

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  qreal m_volume = 0;

  bool m_looping = true;
  void set_volume_internal(qreal p_volume);

  const int m_channelmax = 5;

  HSTREAM m_stream_list[5];
};

#endif // AOSFXPLAYER_H
