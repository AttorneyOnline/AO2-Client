#include "aosfxplayer.h"
#include "file_functions.h"

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
  m_sfxplayer = new QMediaPlayer(m_parent, QMediaPlayer::Flag::LowLatency);
}

AOSfxPlayer::~AOSfxPlayer()
{
  m_sfxplayer->stop();
  m_sfxplayer->deleteLater();
}


void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout)
{
  m_sfxplayer->stop();
  p_sfx = p_sfx.toLower();

  QString misc_path = "";
  QString char_path = "";
  QString sound_path = ao_app->get_sounds_path() + p_sfx;

  if (shout != "")
    misc_path = ao_app->get_base_path() + "misc/" + shout + "/" + p_sfx;
  if (p_char != "")
    char_path = ao_app->get_character_path(p_char) + p_sfx;

  QString f_path;

  if (file_exists(char_path))
      f_path = char_path;
  else if (file_exists(misc_path))
    f_path = misc_path;
  else
    f_path = sound_path;

  m_sfxplayer->setMedia(QUrl::fromLocalFile(f_path));
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
  m_sfxplayer->setVolume(p_value);
}
