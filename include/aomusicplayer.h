#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H
#include "file_functions.h"

#ifdef BASSAUDIO
#include "bass.h"
#include "bassmidi.h"
#include "bassopus.h"
#endif

#include "aoapplication.h"

#include <QDebug>
#include <QWidget>
#include <string.h>
#include <QFuture>
#include <QFutureWatcher>

class AOMusicPlayer {
public:
  AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app);
  virtual ~AOMusicPlayer();
  void set_volume(int p_value, int channel = -1);
  void set_looping(bool toggle, int channel = 0);

  const int m_channelmax = 4;

  // These have to be public for the stupid sync thing
  int loop_start[4] = {0, 0, 0, 0};
  int loop_end[4] = {0, 0, 0, 0};

  QFutureWatcher<QString> music_watcher;

public slots:
  QString play(QString p_song, int channel = 0, bool loop = false,
            int effect_flags = 0);
  void stop(int channel = 0);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  bool m_looping = false;
  int m_volume[4] = {0, 0, 0, 0};

  // Channel 0 = music
  // Channel 1 = ambience
  // Channel 2 = extra
  // Channel 3 = extra
  HSTREAM m_stream_list[4];
  HSYNC loop_sync[4];
};

#endif // AOMUSICPLAYER_H
