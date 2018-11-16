#include "aoblipplayer.h"

AOBlipPlayer::AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_sfxplayer = new QSoundEffect;
  m_parent = parent;
  ao_app = p_ao_app;
}

AOBlipPlayer::~AOBlipPlayer()
{
  m_sfxplayer->stop();
  m_sfxplayer->deleteLater();
}

void AOBlipPlayer::set_blips(QString p_sfx)
{
  m_sfxplayer->stop();
  QString f_path = ao_app->get_sounds_path(p_sfx);
  m_sfxplayer->setSource(QUrl::fromLocalFile(f_path));
  set_volume(m_volume);
}

void AOBlipPlayer::blip_tick()
{
  m_sfxplayer->play();
}

void AOBlipPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  m_sfxplayer->setVolume(p_value / 100.0);
}
