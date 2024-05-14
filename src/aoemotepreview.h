#pragma once

#include "aolayer.h"
#include <QWidget>

class AOEmotePreview : public QWidget
{
  Q_OBJECT

public:
  AOEmotePreview(AOApplication *p_ao_app, QWidget *parent = nullptr);

  void set_widgets();
  void play(QString emote, QString char_name, bool flipped = false, int self_offset = 0, int self_offset_v = 0);

protected:
  void resizeEvent(QResizeEvent *);

private:
  AOApplication *ao_app;

  QString m_emote;
  QString m_char;

  QWidget *ui_viewport;
  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  BackgroundLayer *ui_vp_desk;
  QLabel *ui_size_label;
};
