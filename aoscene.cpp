#include "aoscene.h"

#include "courtroom.h"

#include "file_functions.h"

AOScene::AOScene(Courtroom *parent) : QLabel(parent)
{
  m_courtroom = parent;
}

void AOScene::set_image(QString p_image)
{
  QString background_path = m_courtroom->get_background_path() + p_image;
  QString default_path = m_courtroom->get_default_background_path() + p_image;

  QPixmap background(background_path);
  QPixmap default_bg(default_path);

  int w = this->width();
  int h = this->height();


  if (file_exists(background_path))
    this->setPixmap(background.scaled(w, h));
  else
    this->setPixmap(default_bg.scaled(w, h));
}
