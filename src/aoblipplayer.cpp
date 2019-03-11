#include "aoblipplayer.h"

AOBlipPlayer::AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOBlipPlayer::set_blips(QString p_sfx)
{
  QString f_path = ao_app->get_sounds_path(p_sfx);

  for (int n_stream = 0 ; n_stream < 5 ; ++n_stream)
  {
    #ifdef BASSAUDIO
    BASS_StreamFree(m_stream_list[n_stream]);

    m_stream_list[n_stream] = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
    #endif
  }

  set_volume(m_volume);
}

void AOBlipPlayer::blip_tick()
{
  int f_cycle = m_cycle++;

  if (m_cycle == 5)
    m_cycle = 0;

  HSTREAM f_stream = m_stream_list[f_cycle];
  #ifdef BASSAUDIO
  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(f_stream, BASS_GetDevice());
  BASS_ChannelPlay(f_stream, false);
  #endif
}

void AOBlipPlayer::set_volume(int p_value)
{
  m_volume = p_value;

  float volume = p_value / 100.0f;

  for (int n_stream = 0 ; n_stream < 5 ; ++n_stream)
  {
    #ifdef BASSAUDIO
    BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
    #endif
  }
}
