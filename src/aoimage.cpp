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

bool AOImage::set_image(QString p_path, QString p_misc)
{
  p_path = ao_app->get_image(p_path, ao_app->current_theme, ao_app->get_subtheme(), ao_app->default_theme, p_misc);
  if (!file_exists(p_path)) {
    qDebug() << "Warning: Image" << p_path << "not found! Can't set!";
    return false;
  }
  movie->setFileName(p_path);
  if (movie->frameCount() > 1) {
    movie->start();
  }
  else {
    QPixmap f_pixmap(p_path);

    f_pixmap =
        f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    this->setPixmap(f_pixmap);
    this->setMask(f_pixmap.mask());
  }
  return true;
}
