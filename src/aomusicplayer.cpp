#include "aomusicplayer.h"


AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app): QObject()
{
  m_parent = parent;
  ao_app = p_ao_app;
  music_loop_timer = new QTimer(this);
  music_loop_timer->setSingleShot(true);
  connect(music_loop_timer, SIGNAL(timeout()), this, SLOT(restart_loop()));
}

AOMusicPlayer::~AOMusicPlayer()
{
  kill_loop();
}

void AOMusicPlayer::play(QString p_song)
{
  BASS_ChannelStop(m_stream);

  f_path = ao_app->get_music_path(p_song);

  m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  this->set_volume(m_volume);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  BASS_ChannelPlay(m_stream, false);
  music_loop_timer->stop();
  QWORD len=BASS_ChannelGetLength(m_stream, BASS_POS_BYTE); // the length in bytes
  double time=BASS_ChannelBytes2Seconds(m_stream, len); // the length in seconds
  if(time > 0)
  {
    qDebug() << "Will loop in " << time << " seconds.";
    music_loop_timer->start(time*1000);
  }

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

void AOMusicPlayer::restart_loop()
{
    qDebug() << "Restarting Music";
    /*
    m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);
    if (ao_app->get_audio_output_device() != "default")
      BASS_ChannelSetDevice(m_stream, BASS_GetDevice());*/
    QWORD len=BASS_ChannelGetLength(m_stream, BASS_POS_BYTE); // the length in bytes
    double time=BASS_ChannelBytes2Seconds(m_stream, len); // the length in seconds
    music_loop_timer->start(time*1000);
    BASS_ChannelPlay(m_stream, true);
}

void AOMusicPlayer::kill_loop()
{
    music_loop_timer->stop();
    BASS_ChannelStop(m_stream);
}

