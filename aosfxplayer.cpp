#include "aosfxplayer.h"

#include <string.h>

#include <QDebug>

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::play(QString p_sfx, QString p_char)
{
  BASS_ChannelStop(m_stream);

  p_sfx = p_sfx.toLower();

  QString f_path;

  if (p_char != "")
    f_path = ao_app->get_character_path(p_char) + p_sfx;
  else
    f_path = ao_app->get_sounds_path() + p_sfx;

  m_stream = BASS_StreamCreateFile(FALSE, f_path.toStdString().c_str(), 0, 0, BASS_STREAM_AUTOFREE);

  set_volume(m_volume);

  BASS_ChannelPlay(m_stream, false);
}

void AOSfxPlayer::stop()
{
  BASS_ChannelStop(m_stream);
}

void AOSfxPlayer::set_volume(int p_value)
{
  m_volume = p_value;

  float volume = p_value / 100.0f;

  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);

}
