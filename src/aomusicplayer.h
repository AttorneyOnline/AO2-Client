#pragma once

#include "aoapplication.h"

#include <QFutureWatcher>

class AOMusicPlayer
{
public:
  // 0 = music
  // 1 = ambience
  static constexpr int STREAM_COUNT = 2;

  explicit AOMusicPlayer(AOApplication *ao_app);
  virtual ~AOMusicPlayer();

  void setMuted(bool enabled);

  QString playStream(QString song, int streamId, bool loopEnabled, int effectFlags);

  void setStreamVolume(int value, int streamId);
  void setStreamLooping(bool enabled, int streamId);

  QFutureWatcher<QString> m_watcher;

private:
  AOApplication *ao_app;

  bool m_muted = false;

  int m_volume[STREAM_COUNT]{};
  HSTREAM m_stream_list[STREAM_COUNT]{};
  HSYNC m_loop_sync[STREAM_COUNT]{};
  quint32 m_loop_start[STREAM_COUNT]{};
  quint32 m_loop_end[STREAM_COUNT]{};

  bool ensureValidStreamId(int streamId);
};
