#include "aosfxplayer.h"
#include "file_functions.h"

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::clear()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    BASS_ChannelStop(m_stream_list[n_stream]);
  }
  set_volume_internal(m_volume);
}

void AOSfxPlayer::loop_clear()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    if ((BASS_ChannelFlags(m_stream_list[n_stream], 0, 0) & BASS_SAMPLE_LOOP))
      BASS_ChannelStop(m_stream_list[n_stream]);
  }
  set_volume_internal(m_volume);
}

void AOSfxPlayer::play(QString p_sfx, QString p_character, QString p_misc)
{
  for (int i = 0; i < m_channelmax; ++i) {
    if (BASS_ChannelIsActive(m_stream_list[i]) == BASS_ACTIVE_PLAYING)
      m_channel = (i + 1) % m_channelmax;
    else {
      m_channel = i;
      break;
    }
  }
  QString path = ao_app->get_sfx(p_sfx, p_misc, p_character);
  if (path.endsWith(".opus"))
    m_stream_list[m_channel] = BASS_OPUS_StreamCreateFile(
        FALSE, path.utf16(), 0, 0,
        BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
  else
    m_stream_list[m_channel] = BASS_StreamCreateFile(
        FALSE, path.utf16(), 0, 0,
        BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  set_volume_internal(m_volume);

  BASS_ChannelSetDevice(m_stream_list[m_channel], BASS_GetDevice());
  BASS_ChannelPlay(m_stream_list[m_channel], false);
  BASS_ChannelSetSync(m_stream_list[m_channel], BASS_SYNC_DEV_FAIL, 0,
                      ao_app->BASSreset, 0);
}

void AOSfxPlayer::stop(int channel)
{
  if (channel == -1) {
    channel = m_channel;
  }
  BASS_ChannelStop(m_stream_list[channel]);
}

void AOSfxPlayer::set_volume(qreal p_value)
{
  m_volume = p_value / 100;
  set_volume_internal(m_volume);
}

void AOSfxPlayer::set_volume_internal(qreal p_value)
{
  float volume = static_cast<float>(p_value);
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
  }
}

void AOSfxPlayer::set_looping(bool toggle, int channel)
{
  if (channel == -1) {
    channel = m_channel;
  }
  m_looping = toggle;
  if (BASS_ChannelFlags(m_stream_list[channel], 0, 0) & BASS_SAMPLE_LOOP) {
    if (m_looping == false)
      BASS_ChannelFlags(m_stream_list[channel], 0,
                        BASS_SAMPLE_LOOP); // remove the LOOP flag
  }
  else {
    if (m_looping == true)
      BASS_ChannelFlags(m_stream_list[channel], BASS_SAMPLE_LOOP,
                        BASS_SAMPLE_LOOP); // set the LOOP flag
  }
}
