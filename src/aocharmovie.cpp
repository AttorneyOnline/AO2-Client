#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;
  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);

  ticker = new QTimer(this);
  ticker->setTimerType(Qt::PreciseTimer);
  ticker->setSingleShot(false);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));

//  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix)
{
#ifdef DEBUG_CHARMOVIE
  actual_time.restart();
#endif
  QString emote_path;
  QList<QString> pathlist;
  pathlist = {
      ao_app->get_image_suffix(ao_app->get_character_path(p_char, emote_prefix + p_emote)), //Default path
      ao_app->get_character_path(p_char, p_emote + ".png"),                                 //Non-animated path if emote_prefix fails
      ao_app->get_image_suffix(ao_app->get_theme_path("placeholder")),                      //Theme placeholder path
      ao_app->get_image_suffix(ao_app->get_default_theme_path("placeholder")),              //Default theme placeholder path
  };

  for (QString path : pathlist)
  {
      if (file_exists(path))
      {
          emote_path = path;
          break;
      }
  }

  this->clear();
  ticker->stop();
  preanim_timer->stop();
  movie_frames.clear();
  movie_delays.clear();

  m_reader->setFileName(emote_path);
  QImage f_image = m_reader->read();
  int f_delay = m_reader->nextImageDelay();

  frame = 0;
  max_frames = m_reader->imageCount();

#ifdef DEBUG_CHARMOVIE
  qDebug() << max_frames << "Setting image to " << emote_path << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif

  this->set_frame(movie_frames[frame]);
  this->show();
  if (max_frames > 1)
  {
    movie_frames.append(this->get_pixmap(f_image));
    movie_delays.append(f_delay);
    ticker->start(movie_delays[frame]);
  }
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
//  QString emote_path = ao_app->get_character_path(p_char, p_emote);
  play_once = true;
  play(p_char, p_emote, "");
}

void AOCharMovie::play_talking(QString p_char, QString p_emote)
{
  play_once = false;
  play(p_char, p_emote, "(b)");
}

void AOCharMovie::play_idle(QString p_char, QString p_emote)
{
  play_once = false;
  play(p_char, p_emote, "(a)");
}

void AOCharMovie::stop()
{
  //for all intents and purposes, stopping is the same as hiding. at no point do we want a frozen gif to display
  ticker->stop();
  preanim_timer->stop();
  this->hide();
}

QPixmap AOCharMovie::get_pixmap(QImage image)
{
    QPixmap f_pixmap;
    if(m_flipped)
        f_pixmap = QPixmap::fromImage(image.mirrored(true, false));
    else
        f_pixmap = QPixmap::fromImage(image);
    auto aspect_ratio = Qt::KeepAspectRatio;

    if (f_pixmap.size().width() > f_pixmap.size().height())
      aspect_ratio = Qt::KeepAspectRatioByExpanding;

    if (f_pixmap.size().width() > this->size().width() || f_pixmap.size().height() > this->size().height())
      f_pixmap = f_pixmap.scaled(this->width(), this->height(), aspect_ratio, Qt::SmoothTransformation);
    else
      f_pixmap = f_pixmap.scaled(this->width(), this->height(), aspect_ratio, Qt::FastTransformation);

    return f_pixmap;
}

void AOCharMovie::set_frame(QPixmap f_pixmap)
{
    this->setPixmap(f_pixmap);
    QLabel::move(x + (this->width() - this->pixmap()->width())/2, y);
}

void AOCharMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
//  m_reader->setScaledSize(f_size);
}

void AOCharMovie::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}

void AOCharMovie::movie_ticker()
{
  ++frame;
  if(frame == max_frames)
  {
    if(play_once)
    {
      preanim_done();
      return;
    }
    else
      frame = 0;
  }
//  qint64 difference = elapsed - movie_delays[frame];
  if(frame >= movie_frames.size())
  {
      m_reader->jumpToImage(frame);
      movie_frames.resize(frame + 1);
      movie_frames[frame] = this->get_pixmap(m_reader->read());
      movie_delays.resize(frame + 1);
      movie_delays[frame] = m_reader->nextImageDelay();
  }

#ifdef DEBUG_CHARMOVIE
  qDebug() << frame << movie_delays[frame] << "actual time taken from last frame:" << actual_time.restart();
#endif

  this->set_frame(movie_frames[frame]);
  ticker->setInterval(movie_delays[frame]);
}

void AOCharMovie::preanim_done()
{
  done();
}
