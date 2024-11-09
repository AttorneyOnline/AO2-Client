#include "aomusicplayer.h"

#include "file_functions.h"
#include "options.h"

#include <bass.h>

#include <QDebug>
#include <QFuture>
#include <QWidget>

AOMusicPlayer::AOMusicPlayer(AOApplication *ao_app)
    : ao_app(ao_app)
{}

AOMusicPlayer::~AOMusicPlayer()
{
  for (int n_stream = 0; n_stream < STREAM_COUNT; ++n_stream)
  {
    BASS_ChannelStop(m_stream_list[n_stream]);
  }
}

QString AOMusicPlayer::playStream(QString song, int streamId, bool loopEnabled, int effectFlags)
{
  if (!ensureValidStreamId(streamId))
  {
    return "[ERROR] Invalid Channel";
  }

  quint32 flags = BASS_STREAM_AUTOFREE;
  if (loopEnabled)
  {
    flags |= BASS_SAMPLE_LOOP;
  }

  QString f_path = song;
  HSTREAM newstream;
  if (f_path.startsWith("http"))
  {
    if (!Options::getInstance().streamingEnabled())
    {
      BASS_ChannelStop(m_stream_list[streamId]);
      return QObject::tr("[MISSING] Streaming disabled.");
    }
    QUrl l_url = QUrl(f_path);
    newstream = BASS_StreamCreateURL(l_url.toEncoded().toStdString().c_str(), 0, flags, nullptr, 0);
  }
  else
  {
    flags |= BASS_STREAM_PRESCAN | BASS_UNICODE | BASS_ASYNCFILE;

    f_path = ao_app->get_real_path(ao_app->get_music_path(song));
    newstream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, flags);
  }

  int error = BASS_ErrorGetCode();
  if (Options::getInstance().audioOutputDevice() != "default")
  {
    BASS_ChannelSetDevice(m_stream_list[streamId], BASS_GetDevice());
  }

  m_loop_start[streamId] = 0;
  m_loop_end[streamId] = 0;

  QString d_path = f_path + ".txt";
  if (loopEnabled && file_exists(d_path)) // Contains loop/etc. information file
  {
    QStringList lines = ao_app->read_file(d_path).split("\n");
    bool seconds_mode = false;
    for (QString line : lines)
    {
      QStringList args = line.split("=");
      if (args.size() < 2)
      {
        continue;
      }
      QString arg = args[0].trimmed();
      if (arg == "seconds")
      {
        if (args[1].trimmed() == "true")
        {
          seconds_mode = true; // Use new epic behavior
          continue;
        }

        continue;
      }

      float sample_rate;
      BASS_ChannelGetAttribute(newstream, BASS_ATTRIB_FREQ, &sample_rate);

      // Grab number of bytes for sample size
      int sample_size = 16 / 8;

      // number of channels (stereo/mono)
      int num_channels = 2;

      // Calculate the bytes for loop_start/loop_end to use with the sync proc
      QWORD bytes;
      if (seconds_mode)
      {
        bytes = BASS_ChannelSeconds2Bytes(newstream, args[1].trimmed().toDouble());
      }
      else
      {
        bytes = static_cast<QWORD>(args[1].trimmed().toUInt() * sample_size * num_channels);
      }
      if (arg == "loop_start")
      {
        m_loop_start[streamId] = bytes;
      }
      else if (arg == "loop_length")
      {
        m_loop_end[streamId] = m_loop_start[streamId] + bytes;
      }
      else if (arg == "loop_end")
      {
        m_loop_end[streamId] = bytes;
      }
    }
    qDebug() << "Found data file for song" << song << "length" << BASS_ChannelGetLength(newstream, BASS_POS_BYTE) << "loop start" << m_loop_start[streamId] << "loop end" << m_loop_end[streamId];
  }

  if (BASS_ChannelIsActive(m_stream_list[streamId]) == BASS_ACTIVE_PLAYING)
  {
    DWORD oldstream = m_stream_list[streamId];

    if (effectFlags & SYNC_POS)
    {
      BASS_ChannelLock(oldstream, true);
      // Sync it with the new sample
      BASS_ChannelSetPosition(newstream, BASS_ChannelGetPosition(oldstream, BASS_POS_BYTE), BASS_POS_BYTE);
      BASS_ChannelLock(oldstream, false);
    }

    if ((effectFlags & FADE_OUT) && m_volume[streamId] > 0)
    {
      // Fade out the other sample and stop it (due to -1)
      BASS_ChannelSlideAttribute(oldstream, BASS_ATTRIB_VOL | BASS_SLIDE_LOG, -1, 4000);
    }
    else
    {
      BASS_ChannelStop(oldstream); // Stop the sample since we don't need it anymore
    }
  }
  else
  {
    BASS_ChannelStop(m_stream_list[streamId]);
  }

  m_stream_list[streamId] = newstream;
  BASS_ChannelPlay(newstream, false);
  if (effectFlags & FADE_IN)
  {
    // Fade in our sample
    BASS_ChannelSetAttribute(newstream, BASS_ATTRIB_VOL, 0);
    BASS_ChannelSlideAttribute(newstream, BASS_ATTRIB_VOL, static_cast<float>(m_volume[streamId] / 100.0f), 1000);
  }
  else
  {
    this->setStreamVolume(m_volume[streamId], streamId);
  }

  BASS_ChannelSetSync(newstream, BASS_SYNC_DEV_FAIL, 0, ao_app->BASSreset, 0);

  this->setStreamLooping(loopEnabled, streamId); // Have to do this here due to any
                                                 // crossfading-related changes, etc.

  bool is_stop = (song == "~stop.mp3");
  QString p_song_clear = QUrl(song).fileName();
  p_song_clear = p_song_clear.left(p_song_clear.lastIndexOf('.'));

  if (is_stop && streamId == 0)
  { // don't send text on channels besides 0
    return QObject::tr("None");
  }

  if (error == BASS_ERROR_HANDLE)
  { // Cheap hack to see if file missing
    return QObject::tr("[MISSING] %1").arg(p_song_clear);
  }

  if (song.startsWith("http") && streamId == 0)
  {
    return QObject::tr("[STREAM] %1").arg(p_song_clear);
  }

  if (streamId == 0)
  {
    return p_song_clear;
  }

  return "";
}

void AOMusicPlayer::setMuted(bool enabled)
{
  m_muted = enabled;
  // Update all volume based on the mute setting
  for (int n_stream = 0; n_stream < STREAM_COUNT; ++n_stream)
  {
    setStreamVolume(m_volume[n_stream], n_stream);
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
  // If muted, volume will always be 0
  float volume = (m_volume[streamId] / 100.0f) * !m_muted;
  if (streamId < 0)
  {
    for (int n_stream = 0; n_stream < STREAM_COUNT; ++n_stream)
    {
      BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
    }
  }
  else
  {
    BASS_ChannelSetAttribute(m_stream_list[streamId], BASS_ATTRIB_VOL, volume);
  }
}

void CALLBACK loopProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
  Q_UNUSED(handle);
  Q_UNUSED(data);
  QWORD loop_start = *(static_cast<unsigned *>(user));
  BASS_ChannelLock(channel, true);
  BASS_ChannelSetPosition(channel, loop_start, BASS_POS_BYTE);
  BASS_ChannelLock(channel, false);
}

void AOMusicPlayer::setStreamLooping(bool enabled, int streamId)
{
  if (!ensureValidStreamId(streamId))
  {
    qWarning().noquote() << QObject::tr("Invalid stream ID '%2'").arg(streamId);
    return;
  }

  if (!enabled)
  {
    if (BASS_ChannelFlags(m_stream_list[streamId], 0, 0) & BASS_SAMPLE_LOOP)
    {
      BASS_ChannelFlags(m_stream_list[streamId], 0,
                        BASS_SAMPLE_LOOP); // remove the LOOP flag
    }
    BASS_ChannelRemoveSync(m_stream_list[streamId], m_loop_sync[streamId]);
    m_loop_sync[streamId] = 0;
    return;
  }

  BASS_ChannelFlags(m_stream_list[streamId], BASS_SAMPLE_LOOP,
                    BASS_SAMPLE_LOOP); // set the LOOP flag
  if (m_loop_sync[streamId] != 0)
  {
    BASS_ChannelRemoveSync(m_stream_list[streamId],
                           m_loop_sync[streamId]); // remove the sync
    m_loop_sync[streamId] = 0;
  }

  if (m_loop_start[streamId] < m_loop_end[streamId])
  {
    // Loop when the endpoint is reached.
    m_loop_sync[streamId] = BASS_ChannelSetSync(m_stream_list[streamId], BASS_SYNC_POS | BASS_SYNC_MIXTIME, m_loop_end[streamId], loopProc, &m_loop_start[streamId]);
  }
  else
  {
    // Loop when the end of the file is reached.
    m_loop_sync[streamId] = BASS_ChannelSetSync(m_stream_list[streamId], BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, loopProc, &m_loop_start[streamId]);
  }
}

bool AOMusicPlayer::ensureValidStreamId(int streamId)
{
  return (streamId >= 0 && streamId < STREAM_COUNT);
}
