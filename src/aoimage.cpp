#include "file_functions.h"

#include "aoimage.h"

#include <QBitmap>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
  movie = new QMovie();
  connect(movie, &QMovie::frameChanged, [=]{
    QPixmap f_pixmap = movie->currentPixmap();
    f_pixmap =
        f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    this->setPixmap(f_pixmap);
    this->setMask(f_pixmap.mask());
  });
}

AOImage::~AOImage() {}

bool AOImage::set_image(QString p_image, QString p_misc)
{
  p_image = ao_app->get_image(p_image, ao_app->current_theme, ao_app->get_subtheme(),
                             ao_app->default_theme, p_misc, "", "", !ao_app->get_animated_theme());

  if (!file_exists(p_image)) {
    qDebug() << "Warning: Image" << p_image << "not found! Can't set!";
    return false;
  }
  path = p_image;
  movie->stop();
  movie->setFileName(path);
  if (ao_app->get_animated_theme() && movie->frameCount() > 1) {
    movie->start();
  }
  else {
    QPixmap f_pixmap(path);

    f_pixmap =
        f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    this->setPixmap(f_pixmap);
    this->setMask(f_pixmap.mask());
  }
  return true;
}
