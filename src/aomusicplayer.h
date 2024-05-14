#pragma once

#include "aoapplication.h"

#include <QFutureWatcher>

class AOMusicPlayer
{
public:
  // Channel 0 = music
  // Channel 1 = ambience
  static constexpr int CHANNEL_COUNT = 2;

  AOMusicPlayer(AOApplication *p_ao_app);
  virtual ~AOMusicPlayer();

  void set_volume(int p_value, int channel = -1);
  void set_looping(bool loop_song, int channel = 0);
  void set_muted(bool toggle);

  QFutureWatcher<QString> music_watcher;

public Q_SLOTS:
  QString play(QString p_song, int channel = 0, bool loop = false, int effect_flags = 0);
  void stop(int channel = 0);

private:
  AOApplication *ao_app;

  bool m_muted = false;
  int m_volume[CHANNEL_COUNT] = {0, 0};
  HSTREAM m_stream_list[CHANNEL_COUNT];
  HSYNC m_loop_sync[CHANNEL_COUNT];

  /**
   * @brief The starting sample of the AB-Loop.
   */
  unsigned int m_loop_start[CHANNEL_COUNT] = {0, 0};

  /**
   * @brief The end sample of the AB-Loop.
   */
  unsigned int m_loop_end[CHANNEL_COUNT] = {0, 0};
};
