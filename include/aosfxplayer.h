#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

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
class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
  HSTREAM m_stream;
};
#elif defined(QTAUDIO)
class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);

private:
  QSoundEffect m_sfx;
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
};
#else
class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
};
#endif

#endif // AOSFXPLAYER_H
