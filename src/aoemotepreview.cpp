#include "aoemotepreview.h"

AOEmotePreview::AOEmotePreview(QWidget *parent, AOApplication *p_ao_app) : QWidget(parent)
{
  ao_app = p_ao_app;

  ui_viewport = new QWidget(this);
  ui_vp_player_char = new CharLayer(ui_viewport, ao_app);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_vp_player_char->masked = false;
  ui_vp_background = new BackgroundLayer(ui_viewport, ao_app);
  ui_vp_background->setObjectName("ui_vp_background");
  ui_vp_background->stackUnder(ui_vp_player_char);
  ui_size_label = new QLabel(this);
  ui_size_label->setObjectName("ui_size_label");
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
  setWindowFlag(Qt::Tool);
  this->resize(256, 192);
}

void AOEmotePreview::set_widgets()
{
  ui_viewport->resize(this->width(), this->height());

  ui_vp_player_char->move_and_center(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_background->move_and_center(0, 0);
  ui_vp_background->combo_resize(ui_viewport->width(), ui_viewport->height());
  
  ui_size_label->setText(QString::number(this->width()) + "x" + QString::number(this->height()));

  current_side = ao_app->get_char_side(char_name);
}

void AOEmotePreview::play(QString emote, QString char_name, bool flipped, int self_offset, int self_offset_v)
{
  current_side = ao_app->get_char_side(char_name);
  ui_vp_background->move_and_center(ui_viewport->width(), ui_viewport->height());
  ui_vp_background->load_image(ao_app->get_pos_path(current_side));
  ui_vp_background->combo_resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_player_char->stop();
  ui_vp_player_char->set_flipped(flipped);
  ui_vp_player_char->move_and_center(ui_viewport->width() * self_offset / 100, ui_viewport->height() * self_offset_v / 100);
  ui_vp_player_char->load_image(emote, char_name, 0, false);
  ui_vp_player_char->set_play_once(false);
  m_emote = emote;
  m_char = char_name;
  setWindowTitle(char_name + ": " + emote);
}

void AOEmotePreview::resizeEvent(QResizeEvent *)
{
  set_widgets();
  ui_vp_player_char->load_image(m_emote, m_char, 0, false);
  ui_vp_background->load_image(ao_app->get_pos_path(current_side));
}
