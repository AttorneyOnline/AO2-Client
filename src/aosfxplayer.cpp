#include "aosfxplayer.h"
#include "file_functions.h"

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

#if defined(BASSAUDIO) // Using bass.dll for sfx
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

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout,
                       int channel)
{
  if (channel == -1) {
    if (BASS_ChannelIsActive(m_stream_list[channel]) == BASS_ACTIVE_PLAYING)
      m_channel = (m_channel + 1) % m_channelmax;
    channel = m_channel;
  }

  BASS_ChannelStop(m_stream_list[channel]);

  QString misc_path = "";
  QString char_path = "";
  QString sound_path = ao_app->get_sfx_suffix(ao_app->get_sounds_path(p_sfx));

  if (shout != "")
    misc_path = ao_app->get_sfx_suffix(ao_app->get_base_path() + "misc/" +
                                       shout + "/" + p_sfx);
  if (p_char != "")
    char_path =
        ao_app->get_sfx_suffix(ao_app->get_character_path(p_char, p_sfx));

  QString f_path;

  if (file_exists(char_path))
    f_path = char_path;
  else if (file_exists(misc_path))
    f_path = misc_path;
  else
    f_path = sound_path;

  if (f_path.endsWith(".opus"))
    m_stream_list[channel] = BASS_OPUS_StreamCreateFile(
        FALSE, f_path.utf16(), 0, 0,
        BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
  else
    m_stream_list[channel] = BASS_StreamCreateFile(
        FALSE, f_path.utf16(), 0, 0,
        BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  set_volume_internal(m_volume);

  BASS_ChannelSetDevice(m_stream_list[m_channel], BASS_GetDevice());
  int f_bass_error = BASS_ErrorGetCode();
  if (f_bass_error == BASS_ERROR_DEVICE) {
    ao_app->doBASSreset();
    BASS_ChannelSetDevice(m_stream_list[m_channel], BASS_GetDevice());
  }

  BASS_ChannelPlay(m_stream_list[m_channel], false);
  BASS_ChannelSetSync(m_stream_list[channel], BASS_SYNC_DEV_FAIL, 0,
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
#elif defined(QTAUDIO) // Using Qt's QSoundEffect class

void AOSfxPlayer::clear()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    m_stream_list[n_stream].stop();
  }
  set_volume_internal(m_volume);
}

void AOSfxPlayer::loop_clear()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    m_stream_list[n_stream].stop();
  }
  set_volume_internal(m_volume);
}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout,
                       int channel)
{
  m_stream_list[channel].stop();

  QString misc_path = "";
  QString char_path = "";
  QString sound_path = ao_app->get_sounds_path(p_sfx);

  if (shout != "")
    misc_path = ao_app->get_base_path() + "misc/" + shout + "/" + p_sfx;
  if (p_char != "")
    char_path = ao_app->get_character_path(p_char, p_sfx);

  QString f_path;

  if (file_exists(char_path))
    f_path = char_path;
  else if (file_exists(misc_path))
    f_path = misc_path;
  else
    f_path = sound_path;

  if (file_exists(f_path)) // if its missing, it will glitch out
  {
    m_stream_list[channel].setSource(QUrl::fromLocalFile(f_path));

    set_volume_internal(m_volume);

    m_stream_list[channel].play();
  }
}

void AOSfxPlayer::stop(int channel)
{
  if (channel == -1) {
    channel = m_channel;
  }
  m_stream_list[channel].stop();
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
    m_stream_list[n_stream].setVolume(volume);
  }
}

void AOSfxPlayer::set_looping(bool toggle, int channel)
{
  if (channel == -1) {
    channel = m_channel;
  }
  m_looping = toggle;
  // TODO
}
#else
void AOSfxPlayer::clear() {}

void AOSfxPlayer::loop_clear() {}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout,
                       int channel)
{
}

void AOSfxPlayer::stop(int channel) {}

void AOSfxPlayer::set_volume(qreal p_value) {}

void AOSfxPlayer::set_volume_internal(qreal p_value) {}

void AOSfxPlayer::set_looping(bool toggle, int channel) {}
#endif
