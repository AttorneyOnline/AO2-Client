#include "aosfxplayer.h"

#include "file_functions.h"

AOSfxPlayer::AOSfxPlayer(AOApplication *ao_app)
    : ao_app(ao_app)
{}

int AOSfxPlayer::volume()
{
  return m_volume * 100;
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
    if (BASS_ChannelIsActive(m_stream[i]) == BASS_ACTIVE_PLAYING)
    {
      m_current_stream_id = (i + 1) % STREAM_COUNT;
    }
    else
    {
      m_current_stream_id = i;
      break;
    }
  }

  if (path.endsWith(".opus"))
  {
    m_stream[m_current_stream_id] = BASS_OPUS_StreamCreateFile(FALSE, path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
  }
  else
  {
    m_stream[m_current_stream_id] = BASS_StreamCreateFile(FALSE, path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
  }

  updateInternalVolume();

  BASS_ChannelSetDevice(m_stream[m_current_stream_id], BASS_GetDevice());
  BASS_ChannelPlay(m_stream[m_current_stream_id], false);
  BASS_ChannelSetSync(m_stream[m_current_stream_id], BASS_SYNC_DEV_FAIL, 0, ao_app->BASSreset, 0);
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
    if (BASS_ChannelFlags(m_stream[i], 0, 0) & BASS_SAMPLE_LOOP)
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

  BASS_ChannelStop(m_stream[streamId]);
}

void AOSfxPlayer::setMuted(bool toggle)
{
  m_muted = toggle;
  // Update the audio volume
  updateInternalVolume();
}

void AOSfxPlayer::updateInternalVolume()
{
  float volume = m_muted ? 0.0f : (m_volume * 0.01);
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    BASS_ChannelSetAttribute(m_stream[i], BASS_ATTRIB_VOL, volume);
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
  if (BASS_ChannelFlags(m_stream[streamId], 0, 0) & BASS_SAMPLE_LOOP)
  {
    if (m_looping == false)
    {
      BASS_ChannelFlags(m_stream[streamId], 0,
                        BASS_SAMPLE_LOOP); // remove the LOOP flag
    }
  }
  else
  {
    if (m_looping == true)
    {
      BASS_ChannelFlags(m_stream[streamId], BASS_SAMPLE_LOOP,
                        BASS_SAMPLE_LOOP); // set the LOOP flag
    }
  }
}

int AOSfxPlayer::maybeFetchCurrentStreamId(int streamId)
{
  return streamId == -1 ? m_current_stream_id : streamId;
}

bool AOSfxPlayer::ensureValidStreamId(int streamId)
{
  return streamId >= 0 && streamId < STREAM_COUNT;
}
