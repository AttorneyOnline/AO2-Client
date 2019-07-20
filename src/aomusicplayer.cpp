#include "aomusicplayer.h"

#if defined(BASSAUDIO)
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  BASS_ChannelStop(m_stream);
}

void AOMusicPlayer::play(QString p_song)
{
  BASS_ChannelStop(m_stream);

  QString f_path = ao_app->get_music_path(p_song);

  m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  this->set_volume(m_volume);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  BASS_ChannelPlay(m_stream, false);
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  float volume = m_volume / 100.0f;
  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);
}
#elif defined(QTAUDIO)
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  m_player.stop();
}

void AOMusicPlayer::play(QString p_song)
{
  m_player.stop();

  QString f_path = ao_app->get_music_path(p_song);

  m_player.setMedia(QUrl::fromLocalFile(f_path));

  this->set_volume(m_volume);

  m_player.play();
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  m_player.setVolume(m_volume);
}
#else
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{

}

void AOMusicPlayer::play(QString p_song)
{

}

void AOMusicPlayer::set_volume(int p_value)
{

}
#endif
