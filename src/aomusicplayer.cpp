#include "aomusicplayer.h"

#include "datatypes.h"
#include "file_functions.h"
#include "options.h"

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QDebug>
#include <QEasingCurve>
#include <QEventLoop>
#include <QUrl>
#include <QVariantAnimation>

namespace
{
constexpr int FADE_OUT_MS = 4000;
constexpr int FADE_IN_MS = 1000;
constexpr int LOOP_POLL_INTERVAL_MS = 5;

// Probe the sample rate of an audio file by reading one buffer via QAudioDecoder.
// Returns 0 on failure. Synchronous; only used by the legacy non-seconds loop sidecar form.
int probeSampleRate(const QString &mediaPath)
{
  QAudioDecoder decoder;
  decoder.setSource(QUrl::fromLocalFile(mediaPath));

  int rate = 0;
  bool errored = false;
  QEventLoop loop;

  QObject::connect(&decoder, &QAudioDecoder::bufferReady, &loop, [&]() {
    while (decoder.bufferAvailable())
    {
      QAudioBuffer buf = decoder.read();
      if (buf.format().isValid())
      {
        rate = buf.format().sampleRate();
        loop.quit();
        return;
      }
    }
  });
  QObject::connect(&decoder, &QAudioDecoder::finished, &loop, &QEventLoop::quit);
  QObject::connect(&decoder, qOverload<QAudioDecoder::Error>(&QAudioDecoder::error), &loop,
                   [&](QAudioDecoder::Error) {
                     errored = true;
                     loop.quit();
                   });

  decoder.start();
  loop.exec();
  decoder.stop();

  return errored ? 0 : rate;
}
} // namespace

AOMusicPlayer::AOMusicPlayer(AOApplication *ao_app)
    : ao_app(ao_app)
{}

AOMusicPlayer::~AOMusicPlayer()
{
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    destroyStream(i);
  }
}

bool AOMusicPlayer::ensureValidStreamId(int streamId)
{
  return streamId >= 0 && streamId < STREAM_COUNT;
}

void AOMusicPlayer::destroyStream(int streamId)
{
  if (!ensureValidStreamId(streamId))
  {
    return;
  }
  Stream &s = m_streams[streamId];
  if (s.loopTimer)
  {
    s.loopTimer->stop();
    s.loopTimer->deleteLater();
    s.loopTimer = nullptr;
  }
  if (s.player)
  {
    s.player->stop();
    s.player->deleteLater();
    s.player = nullptr;
  }
  if (s.output)
  {
    s.output->deleteLater();
    s.output = nullptr;
  }
  s.loop_start_ms = 0;
  s.loop_end_ms = 0;
}

void AOMusicPlayer::applyVolume(int streamId)
{
  Stream &s = m_streams[streamId];
  if (!s.output)
  {
    return;
  }
  float volume = m_muted ? 0.0f : (m_volume[streamId] / 100.0f);
  s.output->setVolume(volume);
}

void AOMusicPlayer::fadeOutAndDelete(QMediaPlayer *player, QAudioOutput *output, int durationMs)
{
  auto *anim = new QVariantAnimation(player);
  anim->setStartValue(output->volume());
  anim->setEndValue(0.0f);
  anim->setDuration(durationMs);
  anim->setEasingCurve(QEasingCurve::InExpo);
  QObject::connect(anim, &QVariantAnimation::valueChanged, output,
                   [output](const QVariant &v) { output->setVolume(v.toFloat()); });
  QObject::connect(anim, &QVariantAnimation::finished, player, [player, output]() {
    player->stop();
    player->deleteLater();
    output->deleteLater();
  });
  anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AOMusicPlayer::parseLoopSidecar(int streamId, const QString &dataPath, const QString &mediaPath)
{
  Stream &s = m_streams[streamId];
  s.loop_start_ms = 0;
  s.loop_end_ms = 0;

  QStringList lines = ao_app->read_file(dataPath).split("\n");
  bool seconds_mode = false;
  int sample_rate = 0; // probed lazily; only needed for legacy non-seconds form
  const int sample_size = 2; // 16-bit
  const int num_channels = 2;

  for (const QString &line : lines)
  {
    QStringList args = line.split("=");
    if (args.size() < 2)
    {
      continue;
    }
    QString arg = args[0].trimmed();
    QString val = args[1].trimmed();

    if (arg == "seconds")
    {
      seconds_mode = (val == "true");
      continue;
    }

    qint64 ms = 0;
    if (seconds_mode)
    {
      ms = static_cast<qint64>(val.toDouble() * 1000.0);
    }
    else
    {
      // Legacy form: value is a sample count, converted to ms via probed sample rate.
      if (sample_rate == 0)
      {
        sample_rate = probeSampleRate(mediaPath);
        if (sample_rate == 0)
        {
          qWarning() << "Failed to probe sample rate for" << mediaPath
                     << "— legacy byte-form loop points will be ignored.";
          continue;
        }
      }
      quint64 bytes = static_cast<quint64>(val.toUInt()) * sample_size * num_channels;
      quint64 frame_bytes = static_cast<quint64>(sample_rate) * sample_size * num_channels;
      ms = static_cast<qint64>(bytes * 1000 / frame_bytes);
    }

    if (arg == "loop_start")
    {
      s.loop_start_ms = ms;
    }
    else if (arg == "loop_length")
    {
      s.loop_end_ms = s.loop_start_ms + ms;
    }
    else if (arg == "loop_end")
    {
      s.loop_end_ms = ms;
    }
  }
}

void AOMusicPlayer::armLoopWatcher(int streamId)
{
  Stream &s = m_streams[streamId];
  if (!s.player)
  {
    return;
  }

  if (s.loopTimer)
  {
    s.loopTimer->stop();
    s.loopTimer->deleteLater();
    s.loopTimer = nullptr;
  }

  if (s.loop_start_ms < s.loop_end_ms)
  {
    // Custom loop points: poll position; wrap on cross.
    s.loopTimer = new QTimer(s.player);
    s.loopTimer->setInterval(LOOP_POLL_INTERVAL_MS);
    QMediaPlayer *p = s.player;
    qint64 loopStart = s.loop_start_ms;
    qint64 loopEnd = s.loop_end_ms;
    QObject::connect(s.loopTimer, &QTimer::timeout, p, [p, loopStart, loopEnd]() {
      if (p->position() >= loopEnd)
      {
        p->setPosition(loopStart);
      }
    });
    s.loopTimer->start();
    s.player->setLoops(1);
  }
  else
  {
    // No custom points: let QMediaPlayer handle the loop natively.
    s.player->setLoops(QMediaPlayer::Infinite);
  }
}

QString AOMusicPlayer::playStream(QString song, int streamId, bool loopEnabled, int effectFlags)
{
  if (!ensureValidStreamId(streamId))
  {
    return "[ERROR] Invalid Channel";
  }

  bool isLooping = loopEnabled && !(effectFlags & NO_REPEAT);
  bool isHttp = song.startsWith("http");
  bool is_stop = (song == "~stop.mp3");

  QString p_song_clear = QUrl(song).fileName();
  p_song_clear = p_song_clear.left(p_song_clear.lastIndexOf('.'));

  // Streaming disabled gate.
  if (isHttp && !Options::getInstance().streamingEnabled())
  {
    destroyStream(streamId);
    return QObject::tr("[MISSING] Streaming disabled.");
  }

  // "~stop.mp3" is a sentinel meaning "stop the current music."
  if (is_stop)
  {
    destroyStream(streamId);
    return streamId == 0 ? QObject::tr("None") : QString();
  }

  // Resolve local path and check existence.
  QString resolvedPath = isHttp ? song : ao_app->get_real_path(ao_app->get_music_path(song));
  if (!isHttp && !file_exists(resolvedPath))
  {
    destroyStream(streamId);
    return QObject::tr("[MISSING] %1").arg(p_song_clear);
  }

  Stream &s = m_streams[streamId];
  QMediaPlayer *oldPlayer = s.player;
  QAudioOutput *oldOutput = s.output;
  qint64 oldPositionMs = (oldPlayer && oldPlayer->playbackState() == QMediaPlayer::PlayingState)
                            ? oldPlayer->position()
                            : -1;

  if (s.loopTimer)
  {
    s.loopTimer->stop();
    s.loopTimer->deleteLater();
    s.loopTimer = nullptr;
  }

  // Build the new player.
  auto *player = new QMediaPlayer();
  auto *output = new QAudioOutput();
  player->setAudioOutput(output);

  s.player = player;
  s.output = output;
  s.loop_start_ms = 0;
  s.loop_end_ms = 0;

  // Parse sidecar loop metadata for local files.
  QString sidecar = resolvedPath + ".txt";
  if (isLooping && !isHttp && file_exists(sidecar))
  {
    parseLoopSidecar(streamId, sidecar, resolvedPath);
  }

  player->setSource(isHttp ? QUrl(song) : QUrl::fromLocalFile(resolvedPath));

  // SYNC_POS: seek the new player to the old player's position once it's loaded.
  if (oldPositionMs >= 0 && (effectFlags & SYNC_POS))
  {
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, player,
                     [player, oldPositionMs](QMediaPlayer::MediaStatus status) {
                       if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)
                       {
                         player->setPosition(oldPositionMs);
                       }
                     });
  }

  // Configure looping for the new stream.
  if (isLooping)
  {
    armLoopWatcher(streamId);
  }
  else
  {
    player->setLoops(1);
  }

  // FADE_IN: ramp volume from 0 to target over FADE_IN_MS.
  if (effectFlags & FADE_IN)
  {
    output->setVolume(0.0f);
    auto *anim = new QVariantAnimation(player);
    anim->setStartValue(0.0f);
    float target = m_muted ? 0.0f : (m_volume[streamId] / 100.0f);
    anim->setEndValue(target);
    anim->setDuration(FADE_IN_MS);
    anim->setEasingCurve(QEasingCurve::OutExpo);
    QObject::connect(anim, &QVariantAnimation::valueChanged, output,
                     [output](const QVariant &v) { output->setVolume(v.toFloat()); });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
  }
  else
  {
    applyVolume(streamId);
  }

  player->play();

  // Dispose of the old player. Detach from the Stream struct first so destroyStream
  // can't trample our new player if it runs later.
  if (oldPlayer)
  {
    if ((effectFlags & FADE_OUT) && m_volume[streamId] > 0)
    {
      fadeOutAndDelete(oldPlayer, oldOutput, FADE_OUT_MS);
    }
    else
    {
      oldPlayer->stop();
      oldPlayer->deleteLater();
      if (oldOutput)
      {
        oldOutput->deleteLater();
      }
    }
  }

  if (isHttp && streamId == 0)
  {
    return QObject::tr("[STREAM] %1").arg(p_song_clear);
  }

  if (streamId == 0)
  {
    return p_song_clear;
  }

  return QString();
}

void AOMusicPlayer::setMuted(bool enabled)
{
  m_muted = enabled;
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    applyVolume(i);
  }
}

void AOMusicPlayer::setStreamVolume(int value, int streamId)
{
  if (!ensureValidStreamId(streamId))
  {
    qWarning().noquote() << QObject::tr("Invalid stream ID '%2'").arg(streamId);
    return;
  }
  m_volume[streamId] = value;
  applyVolume(streamId);
}

void AOMusicPlayer::setStreamLooping(bool enabled, int streamId)
{
  if (!ensureValidStreamId(streamId))
  {
    qWarning().noquote() << QObject::tr("Invalid stream ID '%2'").arg(streamId);
    return;
  }

  Stream &s = m_streams[streamId];
  if (!s.player)
  {
    return;
  }

  if (!enabled)
  {
    if (s.loopTimer)
    {
      s.loopTimer->stop();
      s.loopTimer->deleteLater();
      s.loopTimer = nullptr;
    }
    s.player->setLoops(1);
    return;
  }

  armLoopWatcher(streamId);
}
