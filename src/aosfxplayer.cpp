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

  QStringList pathlist;
  pathlist += p_sfx; // The path by itself
  pathlist += ao_app->get_sounds_path(p_sfx); // Sounds folder path
  if (!p_character.isEmpty())
    pathlist += ao_app->get_character_path(p_character, p_sfx); // Character folder
  if (!p_misc.isEmpty() && !ao_app->get_subtheme().isEmpty())
    pathlist += ao_app->get_theme_path("misc/" + p_misc + "/" + p_sfx, ao_app->current_theme + "/" + ao_app->get_subtheme()); // Subtheme misc path
  if (!p_misc.isEmpty())
    pathlist += ao_app->get_theme_path("misc/" + p_misc + "/" + p_sfx, ao_app->current_theme); // Theme misc path
  if (!ao_app->get_subtheme().isEmpty())
    pathlist += ao_app->get_theme_path(p_sfx, ao_app->current_theme + "/" + ao_app->get_subtheme()); // Subtheme path
  if (!p_misc.isEmpty())
    pathlist += ao_app->get_misc_path(p_misc, p_sfx); // Base misc path
  pathlist += ao_app->get_theme_path(p_sfx, ao_app->current_theme); // Theme path
  pathlist += ao_app->get_theme_path(p_sfx, ao_app->default_theme); // Default theme path

  QString path;
  for (QString p : pathlist) {
      p = ao_app->get_case_sensitive_path(ao_app->get_sfx_suffix(p));
      if (file_exists(p)) {
          path = p;
          break;
      }
  }
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
  int f_bass_error = BASS_ErrorGetCode();
  if (f_bass_error == BASS_ERROR_DEVICE) {
    ao_app->doBASSreset();
    BASS_ChannelSetDevice(m_stream_list[m_channel], BASS_GetDevice());
  }

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
