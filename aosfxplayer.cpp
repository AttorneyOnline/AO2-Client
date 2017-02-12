#include "aosfxplayer.h"

#include <string.h>

#include <QDebug>

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

void AOSfxPlayer::play(QString p_sfx, int p_volume)
{
  BASS_ChannelStop(m_stream);

  QString f_path = ao_app->get_sounds_path() + p_sfx;

  qDebug() << "sfx path: " << f_path;

  m_stream = BASS_StreamCreateFile(FALSE, f_path.toStdString().c_str(), 0, 0, BASS_STREAM_AUTOFREE);

  set_volume(p_volume);

  BASS_ChannelPlay(m_stream, false);
}

void AOSfxPlayer::set_volume(int p_value)
{
  float volume = p_value / 100.0f;

  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);

}
