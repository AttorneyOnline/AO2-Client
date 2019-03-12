#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

#if defined(BASSAUDIO)
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
  HSTREAM m_stream_list[5];
};
#elif defined(QTAUDIO)
class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);

  int m_cycle = 0;

private:
  QSoundEffect m_blips;
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume;
  //HSTREAM m_stream_list[5];
};
#else
class AOBlipPlayer
{
public:
  AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

  void set_blips(QString p_sfx);
  void blip_tick();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;
};
#endif


#endif // AOBLIPPLAYER_H
