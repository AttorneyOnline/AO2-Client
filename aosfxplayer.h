#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  HSTREAM m_stream;
};

#endif // AOSFXPLAYER_H
