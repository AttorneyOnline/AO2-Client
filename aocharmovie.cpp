#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

#include <QDebug>

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie(this);
  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);

  this->setMovie(m_movie);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
  m_movie->stop();

  QString pre_path = ao_app->get_character_path(p_char) + p_emote.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";

  if (file_exists(pre_path))
    m_movie->setFileName(pre_path);
  else
    m_movie->setFileName(placeholder_path);

  this->show();
  m_movie->start();
  preanim_timer->start(duration);
}

void AOCharMovie::play_talking(QString p_char, QString p_emote)
{
  m_movie->stop();

  QString talking_path = ao_app->get_character_path(p_char) + "(b)" + p_emote.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";

  if (file_exists(talking_path))
    m_movie->setFileName(talking_path);
  else
    m_movie->setFileName(placeholder_path);

  this->show();
  m_movie->start();

  //D3bug
  m_movie->
}

void AOCharMovie::play_idle(QString p_char, QString p_emote)
{
  m_movie->stop();

  QString idle_path = ao_app->get_character_path(p_char) + "(a)" + p_emote.toLower() + ".gif";
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
  //we'll need this later
  ++n_frame;
}

void AOCharMovie::preanim_done()
{
  done();
}
