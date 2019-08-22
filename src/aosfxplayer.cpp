#include "aosfxplayer.h"
#include "file_functions.h"

#if defined(BASSAUDIO) //Using bass.dll for sfx
AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout)
{
  BASS_ChannelStop(m_stream);
  
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

  m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  set_volume_internal(m_volume);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  BASS_ChannelPlay(m_stream, false);
}

void AOSfxPlayer::stop()
{
  BASS_ChannelStop(m_stream);
}

void AOSfxPlayer::set_volume(qreal p_value)
{
  m_volume = p_value / 100;
  set_volume_internal(m_volume);
}

void AOSfxPlayer::set_volume_internal(qreal p_value)
{
    float volume = p_value;
    BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);
}
#elif defined(QTAUDIO) //Using Qt's QSoundEffect class
AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout)
{
  m_sfx.stop();

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

  if (file_exists(f_path)) //if its missing, it will glitch out
  {
  m_sfx.setSource(QUrl::fromLocalFile(f_path));

  set_volume_internal(m_volume);

  m_sfx.play();
  }
}

void AOSfxPlayer::stop()
{
  m_sfx.stop();
}

void AOSfxPlayer::set_volume(qreal p_value)
{
  m_volume = p_value/100;
  set_volume_internal(m_volume);
}

void AOSfxPlayer::set_volume_internal(qreal p_value)
{
  m_sfx.setVolume(m_volume);
}
#else
AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout)
{

}

void AOSfxPlayer::stop()
{

}

void AOSfxPlayer::set_volume(qreal p_value)
{

}

void AOSfxPlayer::set_volume_internal(qreal p_value)
{

}
#endif
