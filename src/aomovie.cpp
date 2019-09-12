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

  QString shout_path;
  QList<QString> pathlist;
  if (p_gif == "custom")
    pathlist << ao_app->get_image_suffix(ao_app->get_character_path(p_char, p_gif));
  else
    pathlist << ao_app->get_image_suffix(ao_app->get_character_path(p_char, p_gif + "_bubble"));

  pathlist << ao_app->get_image_suffix(ao_app->get_base_path() + "misc/" + p_custom_theme + "/" + p_gif + "_bubble") << //Misc path
              ao_app->get_image_suffix(ao_app->get_custom_theme_path(p_custom_theme, p_gif)) << //Custom theme path
              ao_app->get_image_suffix(ao_app->get_theme_path(p_gif)) << //Theme path
              ao_app->get_image_suffix(ao_app->get_default_theme_path(p_gif)) << //Default theme path
              ao_app->get_image_suffix(ao_app->get_theme_path("placeholder")) << //Placeholder path
              ao_app->get_image_suffix( ao_app->get_default_theme_path("placeholder")); //Default placeholder path

  for (QString path : pathlist)
  {
      if (file_exists(path))
      {
          shout_path = path;
          break;
      }
  }

  m_movie->setFileName(shout_path);

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
  if (n_frame >= (m_movie->frameCount() - 1) && play_once)
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
