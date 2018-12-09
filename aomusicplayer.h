#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "bass.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

private:
  QWidget *m_parent;

  int m_volume = 0;
  HSTREAM m_stream;
};

#endif // AOMUSICPLAYER_H
