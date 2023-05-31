#include "file_functions.h"

#include "aoimage.h"
#include "options.h"

#include <QBitmap>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app, bool make_static) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
  is_static = make_static;
  if (!is_static) // Only create the QMovie if we're non-static
  {
    movie = new QMovie(this);
    connect(movie, &QMovie::frameChanged, [this]{
      QPixmap f_pixmap = movie->currentPixmap();
      f_pixmap =
          f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
      this->setPixmap(f_pixmap);
      if (masked) {
        this->setMask(f_pixmap.mask());
      }
    });
  }
}

AOImage::~AOImage() {}

bool AOImage::set_image(QString p_image, QString p_misc)
{
  QString p_image_resolved = ao_app->get_image(p_image, Options::getInstance().theme(), Options::getInstance().subTheme(),
                                               ao_app->default_theme, p_misc, "", "",
                                               is_static || !Options::getInstance().animatedThemeEnabled());

  if (!file_exists(p_image_resolved)) {
    qWarning() << "could not find image" << p_image;
    return false;
  }

  path = p_image_resolved;
  if (!is_static) {
    movie->stop();
    movie->setFileName(path);
    if (Options::getInstance().animatedThemeEnabled() && movie->frameCount() > 1) {
      movie->start();
    }
  }
  if (is_static || !Options::getInstance().animatedThemeEnabled() || movie->frameCount() <= 1) {
    QPixmap f_pixmap(path);

    f_pixmap =
        f_pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    this->setPixmap(f_pixmap);
    if (masked) {
      this->setMask(f_pixmap.mask());
    }
  }
  return true;
}
