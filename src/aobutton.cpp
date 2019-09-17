#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app) : QPushButton(parent)
{
  ao_app = p_ao_app;
}

AOButton::~AOButton()
{

}

void AOButton::set_image(QString p_image)
{
  QString image_path = ao_app->get_static_image_suffix(ao_app->get_theme_path(p_image));
  QString default_image_path = ao_app->get_static_image_suffix(ao_app->get_default_theme_path(p_image));

  if (file_exists(image_path))
  {
    this->setIcon(QIcon(image_path));
    this->setIconSize(this->size());
    this->setStyleSheet("border:0px");
    this->setText("");
  }
  else if (file_exists(default_image_path))
  {
    this->setIcon(QIcon(default_image_path));
    this->setIconSize(this->size());
    this->setStyleSheet("border:0px");
    this->setText("");
  }
  else
    return;
}

