#include "aoemotepreview.h"

AOEmotePreview::AOEmotePreview(QWidget *parent, AOApplication *p_ao_app) : QWidget(parent)
{
  ao_app = p_ao_app;

  ui_viewport = new QWidget(this);
//  ui_vp_background = new BackgroundLayer(ui_viewport, ao_app);
//  ui_vp_speedlines = new SplashLayer(ui_viewport, ao_app);
  ui_vp_player_char = new CharLayer(ui_viewport, ao_app);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_vp_player_char->masked = false;
//  ui_vp_desk = new BackgroundLayer(ui_viewport, ao_app);
//  ui_vp_desk->setObjectName("ui_vp_desk");
  ui_size_label = new QLabel(this);
  ui_size_label->setObjectName("ui_size_label");
//  ui_size_label->setStyleSheet("QLabel { color: lime; background-color: black; }");
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  setWindowFlag(Qt::Tool);
  this->resize(256, 192);
}

void AOEmotePreview::set_widgets()
{
  ui_viewport->resize(this->width(), this->height());

//  ui_vp_background->move_and_center(0, 0);
//  ui_vp_background->combo_resize(ui_viewport->width(), ui_viewport->height());

//  ui_vp_speedlines->move_and_center(0, 0);
//  ui_vp_speedlines->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_player_char->move_and_center(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  // the AO2 desk element
//  ui_vp_desk->move_and_center(0, 0);
//  ui_vp_desk->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_size_label->setText(QString::number(this->width()) + "x" + QString::number(this->height()));
}

void AOEmotePreview::play(QString emote, QString char_name, bool flipped, int self_offset, int self_offset_v)
{
  ui_vp_player_char->stop();
  ui_vp_player_char->set_play_once(false);
  ui_vp_player_char->set_flipped(flipped);
  ui_vp_player_char->move_and_center(ui_viewport->width() * self_offset / 100, ui_viewport->height() * self_offset_v / 100);
  ui_vp_player_char->load_image(emote, char_name, 0, false);
  m_emote = emote;
  m_char = char_name;
  setWindowTitle(char_name + ": " + emote);
}

void AOEmotePreview::resizeEvent(QResizeEvent *)
{
  set_widgets();
  ui_vp_player_char->load_image(m_emote, m_char, 0, false);
}
