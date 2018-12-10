#include "aomusicplayer.h"
#include "text_file_functions.h"

AOMusicPlayer::AOMusicPlayer(QWidget *parent)
{
  m_parent = parent;
}

AOMusicPlayer::~AOMusicPlayer()
{
  BASS_ChannelStop(m_stream);
}

void AOMusicPlayer::play(QString p_song)
{
  BASS_ChannelStop(m_stream);

  QString f_path = TextFileHandler::get_instance()->get_music_path(p_song);

  m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  this->set_volume(m_volume);

  if (TextFileHandler::get_instance()->get_audio_output_device() != "Default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  BASS_ChannelPlay(m_stream, false);
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  float volume = m_volume / 100.0f;
  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);
}
