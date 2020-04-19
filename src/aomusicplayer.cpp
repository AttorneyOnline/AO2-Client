#include "aomusicplayer.h"

#if defined(BASSAUDIO)
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app): QObject()
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  kill_loop();
}

void AOMusicPlayer::play(QString p_song)
{
  BASS_ChannelStop(m_stream);

  f_path = ao_app->get_music_path(p_song);

  if (p_song.startsWith("http")) {
    m_stream = BASS_StreamCreateURL(f_path.toStdWString().c_str(), 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE, NULL, NULL);
  } else {
    m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
  }

  this->set_volume(m_volume);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  if(enable_looping)
  {
    BASS_ChannelFlags(m_stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
  }
  else
  {
    BASS_ChannelFlags(m_stream, 0, BASS_SAMPLE_LOOP);
  }
  BASS_ChannelPlay(m_stream, false);



}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  float volume = m_volume / 100.0f;
  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);
}

QString AOMusicPlayer::get_path()
{
    return f_path;
}

void AOMusicPlayer::kill_loop()
{
    BASS_ChannelStop(m_stream);
}

#elif defined(QTAUDIO)
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app): QObject()
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

  this->set_volume(100);

  m_player.play();
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;

  qreal linearVolume = QAudio::convertVolume(m_volume / qreal(100),
                                             QAudio::LogarithmicVolumeScale,
                                             QAudio::LinearVolumeScale);

  m_player.setVolume(linearVolume*100);
}

QString AOMusicPlayer::get_path()
{
    return f_path;
}

void AOMusicPlayer::kill_loop()
{
    m_player.stop();
}
#else
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app): QObject()
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

QString AOMusicPlayer::get_path()
{
    return f_path;
}

void AOMusicPlayer::kill_loop()
{

}
#endif
