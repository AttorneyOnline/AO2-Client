#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>
#include <QSoundEffect>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOSfxPlayer();

  void play(QString p_sfx, QString p_char = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  QSoundEffect *m_sfxplayer;

  int m_volume = 0;
};

#endif // AOSFXPLAYER_H
