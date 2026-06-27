#pragma once

#include "aoapplication.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QTimer>

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

private:
  AOApplication *ao_app;

  bool m_muted = false;
  int m_volume[STREAM_COUNT]{};

  struct Stream
  {
    QMediaPlayer *player = nullptr;
    QAudioOutput *output = nullptr;
    QTimer *loopTimer = nullptr;
    qint64 loop_start_ms = 0;
    qint64 loop_end_ms = 0;
  };
  Stream m_streams[STREAM_COUNT];

  void destroyStream(int streamId);
  void parseLoopSidecar(int streamId, const QString &dataPath, const QString &mediaPath);
  void armLoopWatcher(int streamId);
  void applyVolume(int streamId);
  void fadeOutAndDelete(QMediaPlayer *player, QAudioOutput *output, int durationMs);
  bool ensureValidStreamId(int streamId);
};
