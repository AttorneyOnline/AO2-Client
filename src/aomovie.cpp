#include "aomovie.h"

#include "file_functions.h"
#include "courtroom.h"
#include "misc_functions.h"

AOMovie::AOMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie();

  this->setMovie(m_movie);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
}

void AOMovie::set_play_once(bool p_play_once)
{
  play_once = p_play_once;
}

void AOMovie::play(QString p_gif, QString p_char, QString p_custom_theme)
{
  m_movie->stop();

  QString gif_path;

  QString custom_path;
  if (p_gif.contains("custom"))
    custom_path = ao_app->get_image_suffix(ao_app->get_character_path(p_char, p_gif));
  else
    custom_path = ao_app->get_image_suffix(ao_app->get_character_path(p_char, p_gif + "_bubble"));

  QString misc_path = ao_app->get_base_path() + "misc/" + p_custom_theme + "/" + p_gif + "_bubble.gif";
  QString custom_theme_path = ao_app->get_custom_theme_path(p_custom_theme, p_gif + ".gif");
  QString theme_path = ao_app->get_theme_path(p_gif + ".gif");
  QString default_theme_path = ao_app->get_default_theme_path(p_gif + ".gif");
  QString placeholder_path = ao_app->get_theme_path("placeholder.gif");
  QString default_placeholder_path = ao_app->get_default_theme_path("placeholder.gif");

  if (file_exists(custom_path))
    gif_path = custom_path;
  else if (file_exists(misc_path))
    gif_path = misc_path;
  else if (file_exists(custom_theme_path))
    gif_path = custom_theme_path;
  else if (file_exists(theme_path))
    gif_path = theme_path;
  else if (file_exists(default_theme_path))
    gif_path = default_theme_path;
  else if (file_exists(placeholder_path))
    gif_path = placeholder_path;
  else if (file_exists(default_placeholder_path))
    gif_path = default_placeholder_path;
  else
    gif_path = "";

  m_movie->setFileName(gif_path);

  this->show();
  m_movie->start();
}

void AOMovie::stop()
{
  m_movie->stop();
  this->hide();
}

void AOMovie::frame_change(int n_frame)
{
  if (n_frame == (m_movie->frameCount() - 1) && play_once)
  {
    //we need this or else the last frame wont show
    delay(m_movie->nextFrameDelay());

    this->stop();

    //signal connected to courtroom object, let it figure out what to do
    done();
  }
}

void AOMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_movie->setScaledSize(f_size);
}
