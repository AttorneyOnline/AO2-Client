#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie(this);

  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);

  ticker = new QTimer(this);
  ticker->setSingleShot(true);

  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(timer_done()));
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix)
{
  preprocess = false;
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
          if (emote_path.endsWith(".apng"))
              preprocess = true;
          break;
      }
  }

  delete m_movie;
  m_movie = new QMovie(this);
  m_movie->stop();
  this->clear();
  m_movie->setFileName(emote_path);
  m_movie->jumpToFrame(0);
  this->set_frame(m_movie->currentImage());
  qDebug() << "playing file path: " << emote_path;
  this->show();
  ticker->start(m_movie->nextFrameDelay());
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
  QString emote_path = ao_app->get_character_path(p_char, p_emote);

  m_movie->stop();
  this->clear();
  m_movie->setFileName(emote_path);
  m_movie->jumpToFrame(0);

  int full_duration = duration * time_mod;
  int real_duration = 0;

  play_once = false;

  for (int n_frame = 0 ; n_frame < m_movie->frameCount() ; ++n_frame)
  {
    qDebug() << "frame " << n_frame << " delay of " << m_movie->nextFrameDelay();
    real_duration += m_movie->nextFrameDelay();
    m_movie->jumpToFrame(n_frame + 1);
  }

#ifdef DEBUG_GIF
  qDebug() << "full_duration: " << full_duration;
  qDebug() << "real_duration: " << real_duration;
#endif

  double percentage_modifier = 100.0;

  if (real_duration != 0 && duration > 0)
  {
    double modifier = full_duration / static_cast<double>(real_duration);
    percentage_modifier = 100 / modifier;

    if (percentage_modifier > 100.0)
      percentage_modifier = 100.0;
  }

#ifdef DEBUG_GIF
  qDebug() << "% mod: " << percentage_modifier;
#endif

  if (full_duration == 0 || full_duration >= real_duration)
  {
    play_once = true;
  }
  else
  {
    play_once = false;
    preanim_timer->start(full_duration);
  }


  m_movie->setSpeed(static_cast<int>(percentage_modifier));
  play(p_char, p_emote, "");
}

void AOCharMovie::play_talking(QString p_char, QString p_emote)
{
  play_once = false;
  m_movie->setSpeed(100);
  play(p_char, p_emote, "(b)");
}

void AOCharMovie::play_idle(QString p_char, QString p_emote)
{
  play_once = false;
  m_movie->setSpeed(100);
  play(p_char, p_emote, "(a)");
}

void AOCharMovie::stop()
{
  //for all intents and purposes, stopping is the same as hiding. at no point do we want a frozen gif to display
  m_movie->stop();
  preanim_timer->stop();
  this->hide();
}

void AOCharMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_movie->setScaledSize(f_size);
}

void AOCharMovie::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}

void AOCharMovie::movie_ticker()
{
  if(preprocess)
      ticker->start(m_movie->nextFrameDelay());

  if(m_movie->currentFrameNumber() == m_movie->frameCount() - 1)
  {
    QString last_path = m_movie->fileName();
    delete m_movie;
    m_movie = new QMovie(this);
    m_movie->stop();
    this->clear();
    m_movie->setFileName(last_path);
    m_movie->jumpToFrame(0);
    if(play_once)
    {
         timer_done();
    }
  }
  else
  {
    m_movie->jumpToNextFrame();
  }
  this->set_frame(m_movie->currentImage());
//  this->play_frame_sfx();
  if(!preprocess){
      ticker->start(m_movie->nextFrameDelay());
  }
}

void AOCharMovie::set_frame(QImage image)
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
      this->setPixmap(f_pixmap.scaled(this->width(), this->height(), aspect_ratio, Qt::SmoothTransformation));
    else
      this->setPixmap(f_pixmap.scaled(this->width(), this->height(), aspect_ratio, Qt::FastTransformation));

    QLabel::move(x + (this->width() - this->pixmap()->width())/2, y);
}

void AOCharMovie::timer_done()
{

  done();
}
