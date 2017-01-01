#include "file_functions.h"
#include "path_functions.h"
#include "global_variables.h"

#include "aoimage.h"

AOImage::AOImage(QWidget *parent, int x_pos, int y_pos, int x_size, int y_size) : QLabel(parent)
{
  this->move(x_pos, y_pos);
  this->resize(x_size, y_size);
}

AOImage::~AOImage()
{

}

void AOImage::set_image(QString p_image)
{
  QString theme_image_path = get_theme_path() + p_image;
  QString default_image_path = get_base_path() + "themes/default/" + p_image;

  if (file_exists(theme_image_path))
    this->setPixmap(theme_image_path);
  else
    this->setPixmap(default_image_path);
}
