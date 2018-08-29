#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>
#include <QMediaPlayer>

class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  QMediaPlayer *m_player;
  int m_volume = 0;
};

#endif // AOMUSICPLAYER_H
