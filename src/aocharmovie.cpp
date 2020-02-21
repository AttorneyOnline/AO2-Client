#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;
  m_movie = new QMovie(this);
  preanim_timer = new QTimer(this);
  ticker = new QTimer(this);
  preanim_timer->setSingleShot(true);
  ticker->setSingleShot(true);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));
  this->setUpdatesEnabled(true);
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix)
{
  apng = false;
  QString original_path = ao_app->get_character_path(p_char, emote_prefix + p_emote + ".gif");
  QString alt_path = ao_app->get_character_path(p_char, emote_prefix + p_emote + ".png");
  QString apng_path = ao_app->get_character_path(p_char, emote_prefix + p_emote + ".apng");
  QString placeholder_path = ao_app->get_theme_path("placeholder.gif");
  QString placeholder_default_path = ao_app->get_default_theme_path("placeholder.gif");
  QString gif_path;
  current_emote = emote_prefix + p_emote;
  current_char = p_char;
  if (file_exists(apng_path))
  {
    gif_path = apng_path;
    apng = true;
  }
  else if (file_exists(original_path))
    gif_path = original_path;
  else if (file_exists(alt_path))
    gif_path = alt_path;
  else if (file_exists(placeholder_path))
    gif_path = placeholder_path;
  else
    gif_path = placeholder_default_path;
  last_path = gif_path;
  delete m_movie;
  m_movie = new QMovie(this);
  m_movie->stop();
  this->clear();
  m_movie->setFileName(gif_path);
  m_movie->jumpToFrame(0);
  this->LoadImageWithStupidMethodForFlipSupport(m_movie->currentImage());
  this->show();
  this->play_frame_sfx();
  ticker->start(m_movie->nextFrameDelay());
}

void AOCharMovie::play_frame_sfx()
{
  int current_frame = m_movie->currentFrameNumber();
  QString sfx_to_play = ao_app->get_frame_sfx_name(current_char, current_emote, current_frame);
  QString screenshake_to_play = ao_app->get_screenshake_frame(current_char, current_emote, current_frame);
  QString realization_to_play = ao_app->get_realization_frame(current_char, current_emote, current_frame);
  if(sfx_to_play != "" && !use_networked_framehell)
  {
    frame_specific_sfx_player->play(ao_app->get_sfx_suffix(sfx_to_play));
  }
  else if(use_networked_framehell)
  {
      this->sfx_two_network_boogaloo();
  }
  if(screenshake_to_play != "" && !use_networked_framehell)
  {
    mycourtroom->doScreenShake();
  }
  else if(use_networked_framehell)
  {
      this->screenshake_two_network_boogaloo();
  }
  if(realization_to_play != "" && !use_networked_framehell)
  {
    mycourtroom->doRealization();
  }
  else if(use_networked_framehell)
  {
      this->realization_two_network_boogaloo();
  }
}

void AOCharMovie::realization_two_network_boogaloo()
{
    int current_frame = m_movie->currentFrameNumber();
    QStringList realizationList = this->frame_realization_hellstring.split("^");
    for (int i = 0; i < realizationList.length(); i++) {
        QString screenshakeList = realizationList.at(i);
        QStringList extra_garbage = screenshakeList.split("|");
        if(extra_garbage.at(0) != current_emote){
            continue;
        }
        for (int ii = 1; ii < extra_garbage.length(); ii++) {
            QString levels_of_garbage = extra_garbage.at(ii);
            QStringList that_shouldnt_be_possible = levels_of_garbage.split("=");
            if(that_shouldnt_be_possible.at(0).toInt() == current_frame && that_shouldnt_be_possible.at(1) != "") {
                mycourtroom->doRealization();
            }
        }
    }
}

void AOCharMovie::screenshake_two_network_boogaloo()
{
    int current_frame = m_movie->currentFrameNumber();
    QStringList realizationList = this->frame_screenshake_hellstring.split("^");
    for (int i = 0; i < realizationList.length(); i++) {
        QString screenshakeList = realizationList.at(i);
        QStringList extra_garbage = screenshakeList.split("|");
        if(extra_garbage.at(0) != current_emote){
            continue;
        }
        for (int ii = 1; ii < extra_garbage.length(); ii++) {
            QString levels_of_garbage = extra_garbage.at(ii);
            QStringList that_shouldnt_be_possible = levels_of_garbage.split("=");
            if(that_shouldnt_be_possible.at(0).toInt() == current_frame && that_shouldnt_be_possible.at(1) != "") {
                mycourtroom->doScreenShake();
            }
        }
    }
}

void AOCharMovie::sfx_two_network_boogaloo()
{
    int current_frame = m_movie->currentFrameNumber();
    QStringList realizationList = this->frame_sfx_hellstring.split("^");
    for (int i = 0; i < realizationList.length(); i++) {
        QString screenshakeList = realizationList.at(i);
        QStringList extra_garbage = screenshakeList.split("|");
        if(extra_garbage.at(0) != current_emote){
            continue;
        }
        for (int ii = 1; ii < extra_garbage.length(); ii++) {
            QString levels_of_garbage = extra_garbage.at(ii);
            QStringList that_shouldnt_be_possible = levels_of_garbage.split("=");
            if(that_shouldnt_be_possible.at(0).toInt() == current_frame && that_shouldnt_be_possible.at(1) != "") {
                frame_specific_sfx_player->play(ao_app->get_sfx_suffix(that_shouldnt_be_possible.at(1)));
            }
        }
    }
}


void AOCharMovie::movie_ticker()
{
  if(apng){
      ticker->start(m_movie->nextFrameDelay());
  }
  if(m_movie->currentFrameNumber() == m_movie->frameCount() - 1)
  {
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
  this->LoadImageWithStupidMethodForFlipSupport(m_movie->currentImage());
  // imagine if QT had sane stuff like "mirror on QMovie" or "resize the image on QT" or "interface with the current QMovie image" or anything else

  this->play_frame_sfx();
  if(!apng){
      ticker->start(m_movie->nextFrameDelay());
  }

}

void AOCharMovie::LoadImageWithStupidMethodForFlipSupport(QImage image)
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

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
  QString gif_path = ao_app->get_character_path(p_char, p_emote);

  m_movie->stop();
  m_movie->setFileName(gif_path);
  m_movie->jumpToFrame(0);
  int real_duration = 0;
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
  m_movie->stop();
  frame_specific_sfx_player->stop();
  this->hide();
}

void AOCharMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_movie->setScaledSize(this->size());
}
void AOCharMovie::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}

void AOCharMovie::timer_done()
{
  done();
}
