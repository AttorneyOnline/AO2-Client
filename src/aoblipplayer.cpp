#include "aoblipplayer.h"

AOBlipPlayer::AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOBlipPlayer::set_blips(QString p_sfx)
{
  QString f_path = ao_app->get_sfx_suffix(ao_app->get_sounds_path(p_sfx));

  for (int n_stream = 0; n_stream < 5; ++n_stream) {
    BASS_StreamFree(m_stream_list[n_stream]);

    if (f_path.endsWith(".opus"))
      m_stream_list[n_stream] = BASS_OPUS_StreamCreateFile(
          FALSE, f_path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
    else
      m_stream_list[n_stream] = BASS_StreamCreateFile(
          FALSE, f_path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
  }

  set_volume_internal(m_volume);
}

void AOBlipPlayer::blip_tick()
{
  int f_cycle = m_cycle++;

  if (m_cycle == 5)
    m_cycle = 0;

  HSTREAM f_stream = m_stream_list[f_cycle];

  BASS_ChannelSetDevice(f_stream, BASS_GetDevice());
  int f_bass_error = BASS_ErrorGetCode();
  if (f_bass_error == BASS_ERROR_DEVICE) {
    ao_app->doBASSreset();
    BASS_ChannelSetDevice(f_stream, BASS_GetDevice());
  }

  BASS_ChannelPlay(f_stream, false);
}

void AOBlipPlayer::set_volume(int p_value)
{
  m_volume = static_cast<qreal>(p_value) / 100;
  set_volume_internal(m_volume);
}

void AOBlipPlayer::set_volume_internal(qreal p_value)
{
  float volume = static_cast<float>(p_value);

  for (int n_stream = 0; n_stream < 5; ++n_stream) {
    BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
  }
}
