#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(qreal p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  qreal m_volume = 0;

  void set_volume_internal(qreal p_volume);

  #if defined(BASSAUDIO)
  HSTREAM m_stream;
  #elif defined(QTAUDIO)
  QSoundEffect m_sfx;  
  #endif
};

#endif // AOSFXPLAYER_H
