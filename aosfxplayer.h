#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
  HSTREAM m_stream;
};

#endif // AOSFXPLAYER_H
