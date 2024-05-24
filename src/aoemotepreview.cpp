#include "aoemotepreview.h"

AOEmotePreview::AOEmotePreview(AOApplication *ao_app, QWidget *parent)
    : QWidget(parent)
    , ao_app(ao_app)
{
  setWindowFlag(Qt::Tool);
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

  ui_viewport = new QWidget(this);
  ui_vp_player_char = new kal::CharacterAnimationLayer(ao_app, ui_viewport);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_size_label = new QLabel(this);
  ui_size_label->setObjectName("ui_size_label");
}

void AOEmotePreview::updateViewportGeometry()
{
  ui_viewport->resize(size());

  ui_vp_player_char->move(0, 0);
  ui_vp_player_char->resize(ui_viewport->width(), ui_viewport->height());

  ui_size_label->setText(QString::number(ui_viewport->width()) + "x" + QString::number(ui_viewport->height()));
}

void AOEmotePreview::display(QString character, QString emote, kal::CharacterAnimationLayer::EmoteType emoteType, bool flipped, int xOffset, int yOffset)
{
  m_character = character;
  m_emote = emote;
  ui_vp_player_char->stopPlayback();
  ui_vp_player_char->move(ui_viewport->width() * xOffset / 100, ui_viewport->height() * yOffset / 100);
  ui_vp_player_char->loadCharacterEmote(character, emote, emoteType);
  ui_vp_player_char->setPlayOnce(false);
  ui_vp_player_char->setFlipped(flipped);
  ui_vp_player_char->startPlayback();
  setWindowTitle(character + ": " + emote);
}

void AOEmotePreview::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  updateViewportGeometry();
}
