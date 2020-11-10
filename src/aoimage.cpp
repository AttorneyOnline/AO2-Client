#include "file_functions.h"

#include "aoimage.h"

#include <QBitmap>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOImage::~AOImage() {}

bool AOImage::set_image(QString p_image)
{
  QString theme_image_path =
      ao_app->get_static_image_suffix(ao_app->get_theme_path(p_image));
  QString default_image_path =
      ao_app->get_static_image_suffix(ao_app->get_default_theme_path(p_image));

  QString final_image_path;

  if (file_exists(theme_image_path))
    final_image_path = theme_image_path;
  else if (file_exists(default_image_path))
    final_image_path = default_image_path;
  else {
    qDebug() << "Warning: Image" << p_image << "not found! Can't set!";
    return false;
  }

  QPixmap f_pixmap(final_image_path);
  f_pixmap =
      f_pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
  this->setPixmap(f_pixmap);
  this->setMask(f_pixmap.mask());
  return true;
}

bool AOImage::set_chatbox(QString p_path)
{
  p_path = ao_app->get_static_image_suffix(p_path);
  if (!file_exists(p_path)) {
    qDebug() << "Warning: Chatbox" << p_path << "not found! Can't set!";
    return false;
  }

  QPixmap f_pixmap(p_path);

  f_pixmap =
      f_pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
  this->setPixmap(f_pixmap);
  this->setMask(f_pixmap.mask());
  return true;
}
