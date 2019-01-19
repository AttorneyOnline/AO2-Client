#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "bass.h"
#include "aoapplication.h"

#include <QWidget>
#include <string.h>
#include <QDebug>
#include <QTimer>
#include <QObject>

class AOMusicPlayer : public QObject
{
  Q_OBJECT
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  virtual ~AOMusicPlayer();
  void play(QString p_song);
  void set_volume(int p_value);

  void kill_loop();
  QString get_path();

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  QTimer *music_loop_timer;
  int m_volume = 0;
  QString f_path;

  HSTREAM m_stream;

private slots:
  void restart_loop();
};

#endif // AOMUSICPLAYER_H
