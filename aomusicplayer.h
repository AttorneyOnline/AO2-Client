#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "bass.h"
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
  HSTREAM m_stream;
};

#endif // AOMUSICPLAYER_H
