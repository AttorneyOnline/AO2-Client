#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#elif defined(QTAUDIO)
#include <QMediaPlayer>
#endif

#include "aoapplication.h"

#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QWidget>
#include <string.h>

#if defined(BASSAUDIO)
class AOMusicPlayer : public QObject {
  Q_OBJECT
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  virtual ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

  void kill_loop();
  QString get_path();
  bool enable_looping = true;

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
  QString f_path;

  HSTREAM m_stream;
};
#elif defined(QTAUDIO)
class AOMusicPlayer : public QObject {
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

  void kill_loop();
  QString get_path();
  bool enable_looping = true;

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  QMediaPlayer m_player;

  int m_volume = 0;
  QString f_path;
};
#else
class AOMusicPlayer : public QObject {
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  ~AOMusicPlayer();

  void play(QString p_song);
  void set_volume(int p_value);

  void kill_loop();
  QString get_path();
  bool enable_looping = true;

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  int m_volume = 0;
  QString f_path;
};
#endif

#endif // AOMUSICPLAYER_H
