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

void AOEmoteButton::set_on(QString p_char, int p_emote)
{
  //+1 because programmatical emotes start on 0, while filesystem ones start on 1
  QString emotion_number = QString::number(p_emote + 1);
  QString image_path = ao_app->get_character_path(p_char) + "emotions/button" + emotion_number + "_on.png";

  if (file_exists(image_path))
  {
    this->setText("");
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  }
  else
  {
    this->setText(emotion_number);
    this->setStyleSheet("border-image:url(\"\")");
  }
}

void AOEmoteButton::set_off(QString p_char, int p_emote)
{
  QString emotion_number = QString::number(p_emote + 1);
  QString image_path = ao_app->get_character_path(p_char) + "emotions/button" + emotion_number + "_off.png";

  if (file_exists(image_path))
  {
    this->setText("");
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  }
  else
  {
    this->setText(emotion_number);
    this->setStyleSheet("border-image:url(\"\")");
  }
}

void AOEmoteButton::on_clicked()
{
  emote_clicked(m_id);
}
