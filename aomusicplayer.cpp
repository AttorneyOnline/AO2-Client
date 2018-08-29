#include "aomusicplayer.h"

AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
  m_player = new QMediaPlayer;
}

AOMusicPlayer::~AOMusicPlayer()
{
  m_player->stop();
}

void AOMusicPlayer::play(QString p_song)
{
  m_player->setMedia(QUrl::fromLocalFile(ao_app->get_music_path(p_song)));
  this->set_volume(m_volume);
  m_player->play();
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  m_player->setVolume(p_value);
}
