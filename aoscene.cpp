#include "aoscene.h"

#include "courtroom.h"

#include "file_functions.h"

#include <QDebug>

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
  qDebug() << "AOScene width found to be " << w;
  int h = this->height();
  qDebug() << "AOScene height found to be " << h;

  if (file_exists(background_path))
    this->setPixmap(background.scaled(w, h));
  else
    this->setPixmap(default_bg.scaled(w, h));
}

void AOScene::set_legacy_desk(QString p_image)
{
  //vanilla desks vary in both width and height. in order to make that work with viewport rescaling,
  //some INTENSE math is needed.

  QImage f_image(p_image);

  int vp_x = m_courtroom->get_vp_x();
  int vp_y = m_courtroom->get_vp_y();
  int vp_width = m_courtroom->get_vp_w();
  int vp_height = m_courtroom->get_vp_h();

  double y_modifier = 147 / 192;
  double w_modifier = vp_width / 256;
  double h_modifier = vp_height / 192;

  int final_x = vp_x;
  int final_y = vp_y + y_modifier * vp_height;
  int final_w = w_modifier * f_image.width();
  int final_h = h_modifier * f_image.height();

  this->move(final_x, final_y);
  this->resize(final_w, final_h);


}
