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
  ticker->setSingleShot(true);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));

//  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix)
{
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
  m_reader = new QImageReader(emote_path);

  QImage f_image = m_reader->read();
//  while (!f_image.isNull())
//  {
//    if (m_flipped)
//      movie_frames.append(f_image.mirrored(true, false));
//    else
//      movie_frames.append(f_image);
//    f_image = reader->read();
//  }

//  delete m_reader;
  this->show();
  qDebug() << "Setting image to " << f_image;
  this->set_frame(f_image);
  if (m_reader->supportsAnimation())
    ticker->start(m_reader->nextImageDelay());
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
  if(m_reader->format() != "gif")
      ticker->start(m_reader->nextImageDelay());
  if(m_reader->currentImageNumber() == -1)
  {
    if(play_once)
    {
      preanim_done();
      return;
    }
    else
      m_reader->jumpToImage(0); //Loop back
  }
  QImage f_image = m_reader->read();
  if(f_image.isNull())
  {
    qDebug() << "Animated image error on frame" << m_reader->currentImageNumber() << m_reader->errorString();
    stop();
    return;
  }
  this->set_frame(f_image);
  qDebug() << m_reader->format() << "frame" << m_reader->nextImageDelay() << m_reader->currentImageNumber() << m_reader->imageCount();
  if(m_reader->format() == "gif")
      ticker->start(m_reader->nextImageDelay());
}

void AOCharMovie::preanim_done()
{
  done();
}
