#ifndef AOVIEWPORT_H
#define AOVIEWPORT_H

#include "aoapplication.h"
#include "aoevidencedisplay.h"
#include "aolayer.h"

class AOViewport : public QWidget {
  Q_OBJECT

public:
  AOViewport(QWidget *p_parent, AOApplication* p_ao_app = nullptr);
  ~AOViewport(){};

  void setEffectLayerPosition(QString effect_layer_option);

private:
  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  CharLayer *ui_vp_sideplayer_char;
  BackgroundLayer *ui_vp_desk;
  AOEvidenceDisplay *ui_vp_evidence_display;
  QImage *ui_vp_chatbox;
  QLabel *ui_vp_showname;
  InterfaceLayer *ui_vp_chat_arrow;
  SplashLayer *ui_vp_testimony;
  SplashLayer *ui_vp_wtce;
  EffectLayer *ui_vp_effect;
  SplashLayer *ui_vp_objection;
  StickerLayer *ui_vp_sticker;

  AOApplication *ao_app;
};

#endif // AOVIEWPORT_H
