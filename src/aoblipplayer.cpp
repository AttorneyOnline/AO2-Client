#include "aoblipplayer.h"

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QHash>
#include <QMap>
#include <QUrl>

AOBlipPlayer::AOBlipPlayer(AOApplication *ao_app)
    : ao_app(ao_app)
{}

void AOBlipPlayer::setVolume(int value)
{
  m_volume = value;
  updateInternalVolume();
}

void AOBlipPlayer::setMuted(bool enabled)
{
  m_muted = enabled;
  updateInternalVolume();
}

void AOBlipPlayer::setBlip(QString blip)
{
  QString path = ao_app->get_sfx_suffix(ao_app->get_sounds_path(blip));
  QString playable = resolveToPlayablePath(path);
  QUrl source = QUrl::fromLocalFile(playable);
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    m_stream[i].setSource(source);
  }
  updateInternalVolume();
}

void AOBlipPlayer::playBlip()
{
  m_stream[m_cycle].play();
  m_cycle = (m_cycle + 1) % STREAM_COUNT;
}

void AOBlipPlayer::updateInternalVolume()
{
  float volume = m_muted ? 0.0f : (m_volume * 0.01f);
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    m_stream[i].setVolume(volume);
  }
}

namespace
{
// Standard 44-byte WAV header for PCM data.
void writeWavHeader(QFile &out, const QAudioFormat &fmt, qint32 pcmBytes)
{
  const quint32 sampleRate = static_cast<quint32>(fmt.sampleRate());
  const quint16 channels = static_cast<quint16>(fmt.channelCount());
  const quint16 bitsPerSample = static_cast<quint16>(fmt.bytesPerSample() * 8);
  const quint32 byteRate = sampleRate * channels * (bitsPerSample / 8);
  const quint16 blockAlign = channels * (bitsPerSample / 8);
  const quint32 riffSize = 36 + static_cast<quint32>(pcmBytes);

  auto write32 = [&](quint32 v) {
    char b[4] = {char(v & 0xff), char((v >> 8) & 0xff), char((v >> 16) & 0xff), char((v >> 24) & 0xff)};
    out.write(b, 4);
  };
  auto write16 = [&](quint16 v) {
    char b[2] = {char(v & 0xff), char((v >> 8) & 0xff)};
    out.write(b, 2);
  };

  out.write("RIFF", 4);
  write32(riffSize);
  out.write("WAVE", 4);
  out.write("fmt ", 4);
  write32(16);
  write16(1); // PCM
  write16(channels);
  write32(sampleRate);
  write32(byteRate);
  write16(blockAlign);
  write16(bitsPerSample);
  out.write("data", 4);
  write32(static_cast<quint32>(pcmBytes));
}
} // namespace

QString AOBlipPlayer::resolveToPlayablePath(const QString &sourcePath)
{
  if (sourcePath.endsWith(".wav", Qt::CaseInsensitive))
  {
    return sourcePath;
  }

  static QMap<QString, QString> cache;
  auto cached = cache.constFind(sourcePath);
  if (cached != cache.constEnd())
  {
    return *cached;
  }

  QAudioDecoder decoder;
  decoder.setSource(QUrl::fromLocalFile(sourcePath));

  QByteArray pcm;
  QAudioFormat format;
  bool errored = false;

  QEventLoop loop;
  QObject::connect(&decoder, &QAudioDecoder::bufferReady, &loop, [&]() {
    while (decoder.bufferAvailable())
    {
      QAudioBuffer buf = decoder.read();
      if (!format.isValid())
      {
        format = buf.format();
      }
      pcm.append(buf.constData<char>(), buf.byteCount());
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

  if (errored || pcm.isEmpty() || !format.isValid())
  {
    // Decode failed; fall back to the original path so QSoundEffect at least logs a clear error.
    cache.insert(sourcePath, sourcePath);
    return sourcePath;
  }

  QString hash = QString::number(qHash(sourcePath), 16);
  QDir cacheDir(QDir::current().absoluteFilePath("tmp/blip-cache"));
  cacheDir.mkpath(".");
  QString outPath = cacheDir.absoluteFilePath(hash + ".wav");

  QFile out(outPath);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    cache.insert(sourcePath, sourcePath);
    return sourcePath;
  }
  writeWavHeader(out, format, pcm.size());
  out.write(pcm);
  out.close();

  cache.insert(sourcePath, outPath);
  return outPath;
}
