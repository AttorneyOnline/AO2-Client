#include "aomusicplayer.h"

AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    BASS_ChannelStop(m_stream_list[n_stream]);
  }
}

void AOMusicPlayer::play(QString p_song, int channel, bool loop,
                         int effect_flags)
{
  channel = channel % m_channelmax;
  if (channel < 0) // wtf?
    return;
  QString f_path = ao_app->get_music_path(p_song);

  unsigned int flags = BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE |
                       BASS_UNICODE | BASS_ASYNCFILE;
  unsigned int streaming_flags = BASS_STREAM_AUTOFREE;
  if (loop) {
    flags |= BASS_SAMPLE_LOOP;
    streaming_flags |= BASS_SAMPLE_LOOP;
  }

  DWORD newstream;
  if (f_path.startsWith("http")) {
    if (f_path.endsWith(".opus"))
      newstream = BASS_OPUS_StreamCreateURL(f_path.toStdString().c_str(), 0, streaming_flags, nullptr, 0);
    else
      newstream = BASS_StreamCreateURL(f_path.toStdString().c_str(), 0, streaming_flags, nullptr, 0);

  } else {
    if (f_path.endsWith(".opus"))
      newstream = BASS_OPUS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, flags);
    else
      newstream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, flags);
  }


  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream_list[channel], BASS_GetDevice());

  QString d_path = f_path + ".txt";

  loop_start[channel] = 0;
  loop_end[channel] = BASS_ChannelGetLength(newstream, BASS_POS_BYTE);
  if (loop && file_exists(d_path)) // Contains loop/etc. information file
  {
    QStringList lines = ao_app->read_file(d_path).split("\n");
    foreach (QString line, lines) {
      QStringList args = line.split("=");
      if (args.size() < 2)
        continue;
      QString arg = args[0].trimmed();

      float sample_rate;
      BASS_ChannelGetAttribute(newstream, BASS_ATTRIB_FREQ, &sample_rate);

      // Grab number of bytes for sample size
      int sample_size = 16 / 8;

      // number of channels (stereo/mono)
      int num_channels = 2;

      // Calculate the bytes for loop_start/loop_end to use with the sync proc
      QWORD bytes = static_cast<QWORD>(args[1].trimmed().toFloat() *
                                       sample_size * num_channels);
      if (arg == "loop_start")
        loop_start[channel] = bytes;
      else if (arg == "loop_length")
        loop_end[channel] = loop_start[channel] + bytes;
      else if (arg == "loop_end")
        loop_end[channel] = bytes;
    }
    qDebug() << "Found data file for song" << p_song << "length"
             << BASS_ChannelGetLength(newstream, BASS_POS_BYTE) << "loop start"
             << loop_start << "loop end" << loop_end;
  }

  if (BASS_ChannelIsActive(m_stream_list[channel]) == BASS_ACTIVE_PLAYING) {
    DWORD oldstream = m_stream_list[channel];

    if (effect_flags & SYNC_POS) {
      BASS_ChannelLock(oldstream, true);
      // Sync it with the new sample
      BASS_ChannelSetPosition(newstream,
                              BASS_ChannelGetPosition(oldstream, BASS_POS_BYTE),
                              BASS_POS_BYTE);
      BASS_ChannelLock(oldstream, false);
    }

    if ((effect_flags & FADE_OUT) && m_volume[channel] > 0) {
      // Fade out the other sample and stop it (due to -1)
      BASS_ChannelSlideAttribute(oldstream, BASS_ATTRIB_VOL | BASS_SLIDE_LOG,
                                 -1, 4000);
    }
    else
      BASS_ChannelStop(
          oldstream); // Stop the sample since we don't need it anymore
  }
  else
    BASS_ChannelStop(m_stream_list[channel]);

  m_stream_list[channel] = newstream;
  BASS_ChannelPlay(m_stream_list[channel], false);
  if (effect_flags & FADE_IN) {
    // Fade in our sample
    BASS_ChannelSetAttribute(newstream, BASS_ATTRIB_VOL, 0);
    BASS_ChannelSlideAttribute(newstream, BASS_ATTRIB_VOL,
                               static_cast<float>(m_volume[channel] / 100.0f),
                               1000);
  }
  else
    this->set_volume(m_volume[channel], channel);

  BASS_ChannelSetSync(m_stream_list[channel], BASS_SYNC_DEV_FAIL, 0,
                      ao_app->BASSreset, 0);

  this->set_looping(loop, channel); // Have to do this here due to any
                                    // crossfading-related changes, etc.
}

void AOMusicPlayer::stop(int channel)
{
  BASS_ChannelStop(m_stream_list[channel]);
}

void AOMusicPlayer::set_volume(int p_value, int channel)
{
  m_volume[channel] = p_value;
  float volume = m_volume[channel] / 100.0f;
  if (channel < 0) {
    for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
      BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL,
                               volume);
    }
  }
  else {
    BASS_ChannelSetAttribute(m_stream_list[channel], BASS_ATTRIB_VOL, volume);
  }
}

void CALLBACK loopProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
  UNUSED(handle);
  UNUSED(data);
  QWORD loop_start = *(static_cast<unsigned *>(user));
  BASS_ChannelLock(channel, true);
  BASS_ChannelSetPosition(channel, loop_start, BASS_POS_BYTE);
  BASS_ChannelLock(channel, false);
}

void AOMusicPlayer::set_looping(bool toggle, int channel)
{
  m_looping = toggle;
  if (!m_looping) {
    if (BASS_ChannelFlags(m_stream_list[channel], 0, 0) & BASS_SAMPLE_LOOP)
      BASS_ChannelFlags(m_stream_list[channel], 0,
                        BASS_SAMPLE_LOOP); // remove the LOOP flag
    BASS_ChannelRemoveSync(m_stream_list[channel], loop_sync[channel]);
    loop_sync[channel] = 0;
  }
  else {
    BASS_ChannelFlags(m_stream_list[channel], BASS_SAMPLE_LOOP,
                      BASS_SAMPLE_LOOP); // set the LOOP flag
    if (loop_sync[channel] != 0) {
      BASS_ChannelRemoveSync(m_stream_list[channel],
                             loop_sync[channel]); // remove the sync
      loop_sync[channel] = 0;
    }
    if (loop_start[channel] > 0) {
      if (loop_end[channel] == 0)
        loop_end[channel] =
            BASS_ChannelGetLength(m_stream_list[channel], BASS_POS_BYTE);
      if (loop_end[channel] >
          0) // Don't loop zero length songs even if we're asked to
        loop_sync[channel] = BASS_ChannelSetSync(
            m_stream_list[channel], BASS_SYNC_POS | BASS_SYNC_MIXTIME,
            loop_end[channel], loopProc, &loop_start[channel]);
    }
  }
}
