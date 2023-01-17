#ifndef AOVIEWPORT_H
#define AOVIEWPORT_H

#include "aoapplication.h"
#include "aoevidencedisplay.h"
#include "aolayer.h"
#include "datatypes.h"

using namespace AttorneyOnline;

class AOViewport : public QWidget {
  Q_OBJECT

public:
  AOViewport(QWidget *p_parent, AOApplication *p_ao_app = nullptr);
  ~AOViewport(){};

  void setEffectLayerPosition(QString effect_layer_option);

  bool isBusy();

  void startChat(DataTypes::MSPacket f_packet);

  QTimer *getTextStayTimer();

public slots:

  /**
   * @brief onObjectionInterrupt resets the viewport to interrupt the ongoing
   * tick cycle to display the latest incoming message.
   */
  void ObjectionInterrupt();

  /**
   * @brief Sets the background if the viewport is not busy.
   * Otherwise we set it on the next IC message.
   */
  void onBackgroundChanged(QString f_background);

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

  QString m_current_background;

  QTimer m_text_stay;

  enum class AnimationState {
    OBJECTION,
    PREANIM,
    TALKING,
    IDLE,
    NOINT_PREANIM,
    POST_ANIM
  };
  AnimationState anim_state = AnimationState::IDLE;

  enum class TextState { IDLE, TICKING, TICK_DONE };
  AnimationState text_state = AnimationState::IDLE;

  AOApplication *ao_app;
};

#endif // AOVIEWPORT_H
