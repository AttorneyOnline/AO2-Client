#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"
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

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);
  void setLooping(bool is_looping);
private:
  QWidget *m_parent;
  AOApplication *ao_app;
  QTimer *sfx_loop_timer;

  int m_volume = 0;
  bool looping_sfx = false;
  HSTREAM m_stream;
private slots:
  void restart_loop();
};

#endif // AOSFXPLAYER_H
