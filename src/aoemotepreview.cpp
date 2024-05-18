#include "aoemotepreview.h"

AOEmotePreview::AOEmotePreview(AOApplication *ao_app, QWidget *parent)
    : QWidget(parent)
    , ao_app(ao_app)
{
  setWindowFlag(Qt::Tool);
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

  ui_viewport = new QWidget(this);
  ui_vp_player_char = new CharLayer(ao_app, ui_viewport);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_vp_player_char->masked = false;
  ui_size_label = new QLabel(this);
  ui_size_label->setObjectName("ui_size_label");
}

void AOEmotePreview::updateViewportGeometry()
{
  ui_viewport->resize(size());

  ui_vp_player_char->move_and_center(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_size_label->setText(QString::number(width()) + "x" + QString::number(height()));
}

void AOEmotePreview::display(QString character, QString emote, bool flipped, int xOffset, int yOffset)
{
  m_character = character;
  m_emote = emote;
  ui_vp_player_char->stop();
  ui_vp_player_char->set_flipped(flipped);
  ui_vp_player_char->move_and_center(ui_viewport->width() * xOffset / 100, ui_viewport->height() * yOffset / 100);
  ui_vp_player_char->load_image(emote, character, 0, false);
  ui_vp_player_char->set_play_once(false);
  setWindowTitle(character + ": " + emote);
}

void AOEmotePreview::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  updateViewportGeometry();
  ui_vp_player_char->load_image(m_emote, m_character, 0, false);
}
