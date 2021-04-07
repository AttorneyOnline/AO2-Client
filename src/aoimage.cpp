#include "file_functions.h"

#include "aoimage.h"

#include <QBitmap>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app, bool make_static) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
  is_static = make_static;
  if (!is_static) // Only create the QMovie if we're non-static
  {
    movie = new QMovie();
    connect(movie, &QMovie::frameChanged, [=]{
      QPixmap f_pixmap = movie->currentPixmap();
      f_pixmap =
          f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
      this->setPixmap(f_pixmap);
      this->setMask(f_pixmap.mask());
    });
  }
}

AOImage::~AOImage() {}

bool AOImage::set_image(QString p_path, QString p_misc)
{
  // Check if the user wants animated themes
  if (!is_static && ao_app->get_animated_theme())
    // We want an animated image
    p_path = ao_app->get_image(p_path, ao_app->current_theme, ao_app->get_subtheme(), ao_app->default_theme, p_misc);
  else
    // Grab a static variant of the image
    p_path = ao_app->get_image_path(ao_app->get_asset_paths(p_path, ao_app->current_theme, ao_app->get_subtheme(), ao_app->default_theme, p_misc), true);

  if (!file_exists(p_path)) {
    qDebug() << "Warning: Image" << p_path << "not found! Can't set!";
    return false;
  }
  path = p_path;
  if (!is_static) {
    movie->stop();
    movie->setFileName(path);
    if (ao_app->get_animated_theme() && movie->frameCount() > 1) {
      movie->start();
    }
  }
  if (is_static || !ao_app->get_animated_theme() || movie->frameCount() <= 1) {
    QPixmap f_pixmap(path);

    f_pixmap =
        f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    this->setPixmap(f_pixmap);
    this->setMask(f_pixmap.mask());
  }
  return true;
}
