#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H
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
  void set_looping(bool loop_song, int channel = 0);
  void set_muted(bool toggle);

  const int m_channelmax = 4;

  QFutureWatcher<QString> music_watcher;

public slots:
  QString play(QString p_song, int channel = 0, bool loop = false,
            int effect_flags = 0);
  void stop(int channel = 0);

private:
  QWidget *m_parent;
  AOApplication *ao_app;

  bool m_muted = false;
  int m_volume[4] = {0, 0, 0, 0};

  // Channel 0 = music
  // Channel 1 = ambience
  // Channel 2 = extra
  // Channel 3 = extra
  HSTREAM m_stream_list[4];
  HSYNC loop_sync[4];

  /**
   * @brief The starting sample of the AB-Loop.
   */
  unsigned int loop_start[4] = {0, 0, 0, 0};

  /**
   * @brief The end sample of the AB-Loop.
   */
  unsigned int loop_end[4] = {0, 0, 0, 0};
};

#endif // AOMUSICPLAYER_H
