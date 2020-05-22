#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QMediaPlayer>
#endif
#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

#if defined(BASSAUDIO)
class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
  HSTREAM m_stream;
};
#elif defined(QTAUDIO)
class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

private:
  QMediaPlayer m_player;
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
};
#else
class AOMusicPlayer
{
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

private:
  QWidget *m_parent;
  AOApplication *ao_app;
};
#endif

#endif // AOMUSICPLAYER_H
