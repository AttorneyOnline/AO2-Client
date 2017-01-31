#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie(this);

  this->setMovie(m_movie);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
}

void AOCharMovie::set(QString p_char, QString p_pre, QString p_gif)
{
  m_char = p_char;
  m_pre = p_pre;
  m_gif = p_gif;
}

void AOCharMovie::play_pre()
{
  m_movie->stop();

  QString pre_path = ao_app->get_character_path(m_char) + m_pre.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";

  if (file_exists(pre_path))
    m_movie->setFileName(pre_path);
  else
    m_movie->setFileName(placeholder_path);

  this->show();
  m_movie->start();
}

void AOCharMovie::play_talking()
{
  m_movie->stop();

  QString talking_path = ao_app->get_character_path(m_char) + "(b)" + m_gif.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";

  if (file_exists(talking_path))
    m_movie->setFileName(talking_path);
  else
    m_movie->setFileName(placeholder_path);

  this->show();
  m_movie->start();
}

void AOCharMovie::play_idle()
{
  m_movie->stop();

  QString idle_path = ao_app->get_character_path(m_char) + "(a)" + m_gif.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";

  if (file_exists(idle_path))
    m_movie->setFileName(idle_path);
  else
    m_movie->setFileName(placeholder_path);

  this->show();
  m_movie->start();
}

void AOCharMovie::stop()
{
  //for all intents and purposes, stopping is the same as hiding. at no point do we want a frozen gif to display
  m_movie->stop();
  this->hide();
}

void AOCharMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_movie->setScaledSize(f_size);
}

void AOCharMovie::frame_change(int n_frame)
{
  if (n_frame == (m_movie->frameCount() - 1))
  {
    //we need this or else the last frame wont show
    delay(m_movie->nextFrameDelay());

    //signal connected to courtroom object, let it figure out what to do
    done();
  }
}
