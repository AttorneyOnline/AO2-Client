#include "aoblipplayer.h"

#if defined(BASSAUDIO) //Using bass.dll for the blips
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
    BASS_StreamFree(m_stream_list[n_stream]);

    m_stream_list[n_stream] = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
  }

  set_volume_internal(m_volume);
}

void AOBlipPlayer::blip_tick()
{
  int f_cycle = m_cycle++;

  if (m_cycle == 5)
    m_cycle = 0;

  HSTREAM f_stream = m_stream_list[f_cycle];
  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(f_stream, BASS_GetDevice());
  BASS_ChannelPlay(f_stream, false);
}

void AOBlipPlayer::set_volume(qreal p_value)
{
  m_volume = p_value / 100;
  set_volume_internal(m_volume);
}

void AOBlipPlayer::set_volume_internal(qreal p_value)
{
  float volume = p_value;

  for (int n_stream = 0 ; n_stream < 5 ; ++n_stream)
  {
    BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
  }
}
#elif defined(QTAUDIO) //Using Qt's QSoundEffect class
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
    m_blips.setSource(QUrl::fromLocalFile(f_path));
  }

  set_volume_internal(m_volume);
}

void AOBlipPlayer::blip_tick()
{
  int f_cycle = m_cycle++;

  if (m_cycle == 5)
    m_cycle = 0;

  m_blips.play();
}

void AOBlipPlayer::set_volume(qreal p_value)
{
  m_volume = p_value / 100;
  set_volume_internal(m_volume);
}

void AOBlipPlayer::set_volume_internal(qreal p_value)
{
  m_blips.setVolume(m_volume);
}
#else //No audio
AOBlipPlayer::AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOBlipPlayer::set_blips(QString p_sfx)
{

}

void AOBlipPlayer::blip_tick()
{

}

void AOBlipPlayer::set_volume(qreal p_value)
{

}

void AOBlipPlayer::set_volume_internal(qreal p_value)
{

}
#endif
