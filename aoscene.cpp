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

  if (file_exists(background_path))
    this->setPixmap(background_path);
  else
    this->setPixmap(default_path);
}
