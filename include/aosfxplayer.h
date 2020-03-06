#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#include "bassopus.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>
#include <QTimer>

class AOSfxPlayer : public QObject
{
  Q_OBJECT
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void clear();
  void loop_clear();
  void play(QString p_sfx, QString p_char = "", QString shout = "", int channel=-1);
  void stop(int channel=-1);
  void set_volume(qreal p_volume);
  void set_looping(bool is_looping, int channel=-1);
  int m_channel = 0;

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  qreal m_volume = 0;
  const int m_channelmax = 5;
  bool looping_sfx = false;

  bool m_looping = true;
  void set_volume_internal(qreal p_volume);

  #if defined(BASSAUDIO)  
  HSTREAM m_stream_list[5];
  #elif defined(QTAUDIO)
  QSoundEffect m_sfx;
  #endif
};

#endif // AOSFXPLAYER_H
