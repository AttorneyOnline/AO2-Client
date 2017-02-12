#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, int p_volume);
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  HSTREAM m_stream;
};

#endif // AOSFXPLAYER_H
