#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>
#include <QMediaPlayer>

class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOBlipPlayer();

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);

  int m_cycle = 0;

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  QMediaPlayer *m_sfxplayer;

  int m_volume;
};

#endif // AOBLIPPLAYER_H
