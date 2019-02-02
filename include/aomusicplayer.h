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
  bool enable_looping = true;

private:
  QWidget *m_parent;
  AOApplication *ao_app;
  int m_volume = 0;
  QString f_path;

  HSTREAM m_stream;

};

#endif // AOMUSICPLAYER_H
