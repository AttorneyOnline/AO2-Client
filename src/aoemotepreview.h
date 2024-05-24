#pragma once

#include "animationlayer.h"
#include <QWidget>

class AOEmotePreview : public QWidget
{
  Q_OBJECT

public:
  AOEmotePreview(AOApplication *ao_app, QWidget *parent = nullptr);

  void display(QString character, QString emote, kal::CharacterAnimationLayer::EmoteType emoteType, bool flipped = false, int xOffset = 0, int yOffset = 0);

  void updateViewportGeometry();

protected:
  void resizeEvent(QResizeEvent *event);

private:
  AOApplication *ao_app;

  QString m_character;
  QString m_emote;

  QWidget *ui_viewport;
  QLabel *ui_size_label;
  kal::CharacterAnimationLayer *ui_character;
};
