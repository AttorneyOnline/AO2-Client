#pragma once

#include "aolayer.h"
#include <QWidget>

class AOEmotePreview : public QWidget
{
  Q_OBJECT

public:
  AOEmotePreview(AOApplication *ao_app, QWidget *parent = nullptr);

  void display(QString character, QString emote, bool flipped = false, int xOffset = 0, int yOffset = 0);

  void updateViewportGeometry();

protected:
  void resizeEvent(QResizeEvent *event);

private:
  AOApplication *ao_app;

  QString m_character;
  QString m_emote;

  QWidget *ui_viewport;
  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  BackgroundLayer *ui_vp_desk;
  QLabel *ui_size_label;
};
