#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

#include <QDebug>
#include <QImageReader>

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie(this);

  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);

  this->setMovie(m_movie);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(timer_done()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix)
{
  QString original_path = ao_app->get_character_path(p_char) + emote_prefix + p_emote.toLower() + ".gif";
  QString placeholder_path = ao_app->get_theme_path() + "placeholder.gif";
  QString gif_path;

  if (file_exists(original_path))
    gif_path = original_path;
  else
    gif_path = placeholder_path;

  m_movie->stop();
  this->clear();

  m_movie->setFileName(gif_path);

  if (m_flipped)
  {
    QImageReader *reader = new QImageReader(gif_path);

    flipped_movie.clear();
    QImage f_image = reader->read();
    while (!f_image.isNull())
    {
      flipped_movie.append(f_image.mirrored(true, false));
      f_image = reader->read();
    }

    delete reader;
  }
  else
  {
    this->setMovie(m_movie);
  }

  this->show();
  m_movie->start();
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
  if (duration == 0)
    play_once = true;

  else
  {
    play_once = false;
    preanim_timer->start(duration);
  }
  play(p_char, p_emote, "");
}

void AOCharMovie::play_talking(QString p_char, QString p_emote)
{
  play(p_char, p_emote, "(b)");
}

void AOCharMovie::play_idle(QString p_char, QString p_emote)
{
  play(p_char, p_emote, "(a)");
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
  if (m_flipped && flipped_movie.size() > n_frame)
    this->setPixmap(QPixmap::fromImage(flipped_movie.at(n_frame)));

  if (m_movie->frameCount() - 1 == n_frame && play_once)
  {
    delay(m_movie->nextFrameDelay());
    done();
  }
}

void AOCharMovie::timer_done()
{
  done();
}
