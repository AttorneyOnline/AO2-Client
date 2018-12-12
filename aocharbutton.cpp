#include "aocharbutton.h"

#include "file_functions.h"

AOCharButton::AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos, bool is_taken) : QPushButton(parent)
{
  m_parent = parent;

  ao_app = p_ao_app;

  taken = is_taken;

  this->resize(60, 60);
  this->move(x_pos, y_pos);

  ui_taken = new AOImage(this, ao_app);
  ui_taken->resize(60, 60);
  ui_taken->set_image("char_taken.png");
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->hide();

  ui_passworded = new AOImage(this, ao_app);
  ui_passworded->resize(60, 60);
  ui_passworded->set_image("char_passworded");
  ui_passworded->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_passworded->hide();

  ui_selector = new AOImage(parent, ao_app);
  ui_selector->resize(62, 62);
  ui_selector->move(x_pos - 1, y_pos - 1);
  ui_selector->set_image("char_selector.png");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();
}

void AOCharButton::reset()
{
  ui_taken->hide();
  ui_passworded->hide();
  ui_selector->hide();
}

void AOCharButton::set_taken(bool is_taken)
{
  taken = is_taken;
}

void AOCharButton::apply_taken_image()
{
  if (taken)
  {
    ui_taken->move(0,0);
    ui_taken->show();
  }
  else
  {
    ui_taken->hide();
  }
}

void AOCharButton::set_passworded()
{
  ui_passworded->show();
}

void AOCharButton::set_image(QString p_character)
{
  QString image_path = ao_app->get_character_path(p_character, "char_icon.png");

  this->setText("");

  if (file_exists(image_path))
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  else
  {
    this->setStyleSheet("border-image:url()");
    this->setText(p_character);
  }
}

void AOCharButton::enterEvent(QEvent * e)
{
  ui_selector->move(this->x() - 1, this->y() - 1);
  ui_selector->raise();
  ui_selector->show();

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOCharButton::leaveEvent(QEvent * e)
{
  ui_selector->hide();
  QPushButton::leaveEvent(e);
}


