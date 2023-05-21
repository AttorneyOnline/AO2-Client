#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#include "bass.h"
#include "bassopus.h"

#include "aoapplication.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QWidget>
#include <string.h>

class AOBlipPlayer {
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);
  void set_muted(bool toggle);

private:
  QWidget* const m_parent;
  AOApplication* const ao_app;
  qreal m_volume;

  int m_cycle = 0;
  bool m_muted = false;

  void set_volume_internal(qreal p_volume);

  HSTREAM m_stream_list[5];
};

#endif // AOBLIPPLAYER_H
