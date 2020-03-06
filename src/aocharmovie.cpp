#include "aocharmovie.h"

#include "misc_functions.h"
#include "file_functions.h"
#include "aoapplication.h"

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;
  //m_movie = new QMovie(this);
  preanim_timer = new QTimer(this);
  ticker = new QTimer(this);
  preanim_timer->setSingleShot(true);

  ticker->setTimerType(Qt::PreciseTimer);
  ticker->setSingleShot(false);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));

//  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOCharMovie::load_image(QString p_char, QString p_emote, QString emote_prefix)
{
#ifdef DEBUG_CHARMOVIE
  actual_time.restart();
#endif
  QString emote_path;
  QList<QString> pathlist;
  pathlist = {
      ao_app->get_image_suffix(ao_app->get_character_path(p_char, emote_prefix +        p_emote)),  //Default path
      ao_app->get_image_suffix(ao_app->get_character_path(p_char, emote_prefix + "/" +  p_emote)),  //Path check if it's categorized into a folder
      ao_app->get_image_suffix(ao_app->get_character_path(p_char,                       p_emote)),  //Non-animated path if emote_prefix fails
      ao_app->get_image_suffix(ao_app->get_theme_path("placeholder")),          //Theme placeholder path
      ao_app->get_image_suffix(ao_app->get_default_theme_path("placeholder")),  //Default theme placeholder path
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
  movie_effects.clear();

  m_reader->setFileName(emote_path);
  QPixmap f_pixmap = this->get_pixmap(m_reader->read());
  int f_delay = m_reader->nextImageDelay();

  frame = 0;
  max_frames = m_reader->imageCount();

  this->set_frame(f_pixmap);
  this->show();
  if (max_frames > 1)
  {
    movie_frames.append(f_pixmap);
    movie_delays.append(f_delay);
  }

  m_char = p_char;
  m_emote = emote_prefix + p_emote;

  if (network_strings.size() > 0) //our FX overwritten by networked ones
    this->load_network_effects();
  else //Use default ini FX
    this->load_effects();
#ifdef DEBUG_CHARMOVIE
  qDebug() << max_frames << "Setting image to " << emote_path << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif
}

void AOCharMovie::load_effects()
{
  movie_effects.clear();
  movie_effects.resize(max_frames);
  for (int e_frame = 0; e_frame < max_frames; ++e_frame)
  {
    QString effect = ao_app->get_screenshake_frame(m_char, m_emote, e_frame);
    if (effect != "")
    {
      movie_effects[e_frame].append("shake");
    }

    effect = ao_app->get_flash_frame(m_char, m_emote, e_frame);
    if (effect != "")
    {
      movie_effects[e_frame].append("flash");
    }

    effect = ao_app->get_sfx_frame(m_char, m_emote, e_frame);
    if (effect != "")
    {
      movie_effects[e_frame].append("sfx^"+effect);
    }
  }
}

void AOCharMovie::load_network_effects()
{
  movie_effects.clear();
  movie_effects.resize(max_frames);
  //Order is important!!!
  QStringList effects_list = {"shake", "flash", "sfx^"};

  //Determines which list is smaller - effects_list or network_strings - and uses it as basis for the loop.
  //This way, incomplete network_strings would still be parsed, and excess/unaccounted for networked information is omitted.
  int effects_size = qMin(effects_list.size(), network_strings.size());

  for (int i = 0; i < effects_size; ++i)
  {
    QString netstring = network_strings.at(i);
    QStringList emote_splits = netstring.split("^");
    foreach (QString emote, emote_splits)
    {
      QStringList parsed = emote.split("|");
      if (parsed.size() <= 0 || parsed.at(0) != m_emote)
        continue;
      foreach (QString frame_data, parsed)
      {
        QStringList frame_split = frame_data.split("=");
        if (frame_split.size() <= 1) //We might still be hanging at the emote itself (entry 0).
          continue;
        int f_frame = frame_split.at(0).toInt();
        if (f_frame >= max_frames)
        {
          qDebug() << "Warning: out of bounds" << effects_list[i] << "frame" << f_frame << "out of" << max_frames << "for" << m_char << m_emote;
          continue;
        }
        QString f_data = frame_split.at(1);
        if (f_data != "")
        {
          QString effect = effects_list[i];
          if (effect == "sfx^") //Currently the only frame result that feeds us data, let's yank it in.
            effect += f_data;
          qDebug() << effect << f_data << "frame" << f_frame << "for" << m_char << m_emote;
          movie_effects[f_frame].append(effect);
        }
      }
    }
   }
}

void AOCharMovie::play()
{
  play_frame_effect(frame);
  if (max_frames <= 1)
  {
    if (play_once)
      ticker->start(60);
  }
  else
    ticker->start(this->get_frame_delay(movie_delays[frame]));
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration)
{
  load_image(p_char, p_emote, "");
  //As much as I'd like to screw around with [Time] durations modifying the animation speed, I don't think I can reliably do that,
  //not without looping through all frames in the image at least - which causes lag. So for now it simply ends the preanimation early instead.
  play_once = true;
  if (duration > 0) //It's -1 if there's no definition in [Time] for it. In which case, it will let the animation run out in full. Duration 0 does the same.
    preanim_timer->start(duration * time_mod); //This timer will not fire if the animation finishes earlier than that
  play();
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

void AOCharMovie::play_talking(QString p_char, QString p_emote)
{
  play_once = false;
  load_image(p_char, p_emote, "(b)");
  play();
}

void AOCharMovie::play_idle(QString p_char, QString p_emote)
{
  play_once = false;
  load_image(p_char, p_emote, "(a)");
  play();
}

void AOCharMovie::play_frame_effect(int frame)
{
  if(frame < max_frames)
  {
    foreach (QString effect, movie_effects[frame])
    {
      if(effect == "shake")
      {
        shake();
#ifdef DEBUG_CHARMOVIE
        qDebug() << "Attempting to play shake on frame" << frame;
#endif
      }

      if(effect == "flash")
      {
        flash();
#ifdef DEBUG_CHARMOVIE
        qDebug() << "Attempting to play flash on frame" << frame;
#endif
      }

      if(effect.startsWith("sfx^"))
      {
        QString sfx = effect.section("^", 1);
        play_sfx(sfx);
#ifdef DEBUG_CHARMOVIE
        qDebug() << "Attempting to play sfx" << sfx << "on frame" << frame;
#endif
      }
    }
  }
 // play(p_char, p_emote, "(a)");
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
//    auto aspect_ratio = Qt::KeepAspectRatio;
    auto transform_mode = Qt::SmoothTransformation;

    f_pixmap = f_pixmap.scaledToHeight(f_h, transform_mode);
    this->resize(f_pixmap.size());

    return f_pixmap;
}

void AOCharMovie::set_frame(QPixmap f_pixmap)
{
    this->setPixmap(f_pixmap);
    QLabel::move(x + (f_w - f_pixmap.width())/2, y + (f_h - f_pixmap.height())); //Always center horizontally, always put at the bottom vertically
}

int AOCharMovie::get_frame_delay(int delay)
{
    return static_cast<int>(double(delay) * double(speed/100));
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
  if(frame >= max_frames)
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
  play_frame_effect(frame);
  ticker->setInterval(this->get_frame_delay(movie_delays[frame]));
}

void AOCharMovie::preanim_done()
{
  ticker->stop();
  preanim_timer->stop();
  done();
}
  
void AOCharMovie::timer_done()
{
  done();
}
