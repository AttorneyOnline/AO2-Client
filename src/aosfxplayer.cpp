#include "aosfxplayer.h"

#include "file_functions.h"

#include <QDebug>
#include <QUrl>

AOSfxPlayer::AOSfxPlayer(AOApplication *ao_app)
    : ao_app(ao_app)
{}

int AOSfxPlayer::volume()
{
  return m_volume;
}

void AOSfxPlayer::setVolume(int value)
{
  m_volume = value;
  updateInternalVolume();
}

void AOSfxPlayer::play(QString path)
{
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    if (m_stream[i].isPlaying())
    {
      m_current_stream_id = (i + 1) % STREAM_COUNT;
    }
    else
    {
      m_current_stream_id = i;
      break;
    }
  }

  QSoundEffect &stream = m_stream[m_current_stream_id];
  stream.setLoopCount(1);
  stream.setSource(QUrl::fromLocalFile(path));
  updateInternalVolume();
  stream.play();
}

void AOSfxPlayer::findAndPlaySfx(QString sfx)
{
  // TODO replace this with proper pathing tools
  findAndPlayCharacterShout(sfx, QString(), QString());
}

void AOSfxPlayer::findAndPlayCharacterSfx(QString sfx, QString character)
{
  // TODO replace this with proper pathing tools
  findAndPlayCharacterShout(sfx, character, QString());
}

void AOSfxPlayer::findAndPlayCharacterShout(QString shout, QString character, QString group)
{
  QString file_path = ao_app->get_sfx(shout, group, character);
  if (file_exists(file_path))
  {
    play(file_path);
  }
}

void AOSfxPlayer::stopAll()
{
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    stop(i);
  }
}

void AOSfxPlayer::stopAllLoopingStream()
{
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    if (m_stream[i].loopCount() == QSoundEffect::Infinite)
    {
      stop(i);
    }
  }
}

void AOSfxPlayer::stop(int streamId)
{
  streamId = maybeFetchCurrentStreamId(streamId);
  if (!ensureValidStreamId(streamId))
  {
    qWarning().noquote() << QObject::tr("Failed to stop stream; invalid stream ID '%1'").arg(streamId);
    return;
  }

  m_stream[streamId].stop();
}

void AOSfxPlayer::setMuted(bool toggle)
{
  m_muted = toggle;
  updateInternalVolume();
}

void AOSfxPlayer::updateInternalVolume()
{
  float volume = m_muted ? 0.0f : (m_volume * 0.01f);
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    m_stream[i].setVolume(volume);
  }
}

void AOSfxPlayer::setLooping(bool toggle, int streamId)
{
  streamId = maybeFetchCurrentStreamId(streamId);
  if (!ensureValidStreamId(streamId))
  {
    qWarning().noquote() << QObject::tr("Failed to setup stream loop; invalid stream ID '%1'").arg(streamId);
    return;
  }

  m_looping = toggle;
  m_stream[streamId].setLoopCount(m_looping ? QSoundEffect::Infinite : 1);
}

int AOSfxPlayer::maybeFetchCurrentStreamId(int streamId)
{
  return streamId == -1 ? m_current_stream_id : streamId;
}

bool AOSfxPlayer::ensureValidStreamId(int streamId)
{
  return streamId >= 0 && streamId < STREAM_COUNT;
}
