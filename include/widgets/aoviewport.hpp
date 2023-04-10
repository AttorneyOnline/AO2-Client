#ifndef AOVIEWPORT_H
#define AOVIEWPORT_H
#pragma once

#include "interfaces/widgets/viewport.hpp"

class QTimer;
class QLabel;
class QTextEdit;

class AOApplication;
class Courtroom;

class AOEvidenceDisplay;
class AOImage;
class BackgroundLayer;
class CharLayer;
class EffectLayer;
class InterfaceLayer;
class SplashLayer;
class StickerLayer;


class AOViewport : public AttorneyOnline::UI::Viewport {
public:
  AOViewport(QWidget *parent, AOApplication* f_ao_app, Courtroom* f_courtroom);
  ~AOViewport();

  bool isViewportBusy() override;

  enum class AnimationState {
    OBJECTION,
    PREANIM,
    TALKING,
    POST,
    IDLE,
    NOINTPRE
  };

  enum class TextState { IDLE, TICKING, DONE };

private:
  AOApplication* ao_app;
  Courtroom* courtroom;

  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  CharLayer *ui_vp_sideplayer_char;
  BackgroundLayer *ui_vp_desk;
  AOEvidenceDisplay *ui_vp_evidence_display;
  AOImage *ui_vp_chatbox;
  QLabel *ui_vp_showname;
  InterfaceLayer *ui_vp_chat_arrow;
  QTextEdit *ui_vp_message;
  SplashLayer *ui_vp_testimony;
  SplashLayer *ui_vp_wtce;
  EffectLayer *ui_vp_effect;
  SplashLayer *ui_vp_objection;
  StickerLayer *ui_vp_sticker;

  AnimationState m_anim_state = AnimationState::IDLE;
  TextState m_text_state = TextState::IDLE;

  QTimer *m_text_stay_timer;
};

#endif // AOVIEWPORT_H
