#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QDebug>
#include <QWidget>
#include <string.h>

class AOSfxPlayer {
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;
#if defined(BASSAUDIO)
  HSTREAM m_stream;
#elif defined(QTAUDIO)
  QSoundEffect m_sfx;
#endif
  int m_volume = 0;
};

#endif // AOSFXPLAYER_H
