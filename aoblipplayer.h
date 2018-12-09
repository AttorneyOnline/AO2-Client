#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#include "bass.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);

  int m_cycle = 0;

private:
  QWidget *m_parent;

  int m_volume;
  HSTREAM m_stream_list[5];
};

#endif // AOBLIPPLAYER_H
