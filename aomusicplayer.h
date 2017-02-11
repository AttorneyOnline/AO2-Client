#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>

class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_song);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  HSTREAM m_stream;
};

#endif // AOMUSICPLAYER_H
