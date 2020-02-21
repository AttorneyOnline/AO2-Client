#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>


class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(qreal p_volume);

  int m_cycle = 0;

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  qreal m_volume;

  void set_volume_internal(qreal p_volume);

  #if defined(BASSAUDIO)
  HSTREAM m_stream_list[5];
  #elif defined(QTAUDIO)
  QSoundEffect m_blips;
  #endif
};

#endif // AOBLIPPLAYER_H
