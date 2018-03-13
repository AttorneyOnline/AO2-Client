#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>

#define blip_count 5

class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);

  int m_cycle = 0;

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume;
  HSTREAM m_stream_list[blip_count];
};

#endif // AOBLIPPLAYER_H
