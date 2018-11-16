#include "aosfxplayer.h"

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_sfxplayer = new QSoundEffect;
  m_parent = parent;
  ao_app = p_ao_app;
}

AOSfxPlayer::~AOSfxPlayer()
{
  m_sfxplayer->stop();
  m_sfxplayer->deleteLater();
}

void AOSfxPlayer::play(QString p_sfx, QString p_char)
{
  m_sfxplayer->stop();
  p_sfx = p_sfx;
  QString f_path;
  if (p_char != "")
    f_path = ao_app->get_character_path(p_char, p_sfx);
  else
    f_path = ao_app->get_sounds_path(p_sfx);

  m_sfxplayer->setSource(QUrl::fromLocalFile(f_path));
  set_volume(m_volume);

  m_sfxplayer->play();
}

void AOSfxPlayer::stop()
{
  m_sfxplayer->stop();
}

void AOSfxPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  m_sfxplayer->setVolume(p_value / 100.0);
}
