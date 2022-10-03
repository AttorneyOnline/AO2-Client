#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "options.h"

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app)
    : QPushButton(parent)
{
  ao_app = p_ao_app;
  movie = new QMovie(this);
  connect(movie, &QMovie::frameChanged, [this]{
    this->setIcon(movie->currentPixmap().scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setIconSize(QSize(this->width(), this->height()));
  });
}

AOButton::~AOButton() {}

void AOButton::set_image(QString p_path, QString p_misc)
{
  movie->stop();
  QString p_image;
  p_image = ao_app->get_image(p_path, Options::getInstance().theme(), Options::getInstance().subTheme(),
                              ao_app->default_theme, p_misc, "", "", !Options::getInstance().animatedThemeEnabled());
  if (p_image.isEmpty()) {
      this->setIcon(QIcon());
      this->setIconSize(this->size());
      this->setStyleSheet("");
      return;
  }
  this->setText("");
  this->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
  movie->setFileName(p_image);
  // We double-check if the user wants animated themes, so even if an animated image slipped through,
  // we still set it static
  if (Options::getInstance().animatedThemeEnabled() && movie->frameCount() > 1) {
    movie->start();
  }
  else {
    this->setIcon(QPixmap(p_image).scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setIconSize(this->size());
  }
}
