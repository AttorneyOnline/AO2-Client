#include "aoemotebutton.h"

#include "file_functions.h"

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y) : QPushButton(p_parent)
{
  parent = p_parent;
  ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize(40, 40);

  connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
}

void AOEmoteButton::set_image(QString p_image, QString p_emote_comment)
{
  if (file_exists(p_image))
  {
    this->setIcon(QIcon(p_image));
    this->setIconSize(this->size());
    this->setStyleSheet("border:0px");
    this->setText("");
  }
  else
  {
    this->setText(p_emote_comment);
    this->setStyleSheet("border-image:url(\"\")");
  }
}

void AOEmoteButton::set_char_image(QString p_char, int p_emote, QString suffix)
{
  QString emotion_number = QString::number(p_emote + 1);
  QString image_path = ao_app->get_static_image_suffix(ao_app->get_character_path(p_char, "emotions/button" + emotion_number + suffix));

  this->set_image(image_path, ao_app->get_emote_comment(p_char, p_emote));
}

void AOEmoteButton::on_clicked()
{
  emote_clicked(m_id);
}
