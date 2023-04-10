#include "widgets/aoviewport.hpp"

#include <QTimer>
#include <QTextEdit>

#include "aolayer.h"
#include "aoimage.h"
#include "aoevidencedisplay.h"

AOViewport::AOViewport(QWidget *parent, AOApplication *f_ao_app,
                       Courtroom *f_courtroom)
    : AttorneyOnline::UI::Viewport(parent), ao_app(f_ao_app),
      courtroom(f_courtroom)
{
  ui_vp_background = new BackgroundLayer(this, ao_app);
  ui_vp_background->setObjectName("ui_vp_background");
  ui_vp_speedlines = new SplashLayer(this, ao_app);
  ui_vp_speedlines->setObjectName("ui_vp_speedlines");
  ui_vp_player_char = new CharLayer(this, ao_app);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_vp_player_char->masked = false;
  ui_vp_sideplayer_char = new CharLayer(this, ao_app);
  ui_vp_sideplayer_char->setObjectName("ui_vp_sideplayer_char");
  ui_vp_sideplayer_char->masked = false;
  ui_vp_sideplayer_char->hide();
  ui_vp_desk = new BackgroundLayer(this, ao_app);
  ui_vp_desk->setObjectName("ui_vp_desk");

  ui_vp_effect = new EffectLayer(this, ao_app);
  ui_vp_effect->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_effect->setObjectName("ui_vp_effect");

  ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);
  ui_vp_evidence_display->setObjectName("ui_vp_evidence_display");

  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_chatbox->setObjectName("ui_vp_chatbox");

  ui_vp_sticker = new StickerLayer(this, ao_app);
  ui_vp_sticker->set_play_once(false);
  ui_vp_sticker->set_cull_image(false);
  ui_vp_sticker->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_sticker->setObjectName("ui_vp_sticker");

  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_showname->setObjectName("ui_vp_showname");
  ui_vp_showname->setAlignment(Qt::AlignLeft);
  ui_vp_chat_arrow = new InterfaceLayer(this, ao_app);
  ui_vp_chat_arrow->set_play_once(false);
  ui_vp_chat_arrow->set_cull_image(false);
  ui_vp_chat_arrow->setObjectName("ui_vp_chat_arrow");

  ui_vp_message = new QTextEdit(this);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);
  ui_vp_message->setObjectName("ui_vp_message");

  ui_vp_testimony = new SplashLayer(this, ao_app);
  ui_vp_testimony->set_play_once(false);
  ui_vp_testimony->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_testimony->setObjectName("ui_vp_testimony");
  ui_vp_wtce = new SplashLayer(this, ao_app);
  ui_vp_wtce->set_play_once(true);
  ui_vp_wtce->continuous = false;
  ui_vp_wtce->force_continuous = true;
  ui_vp_wtce->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_wtce->setObjectName("ui_vp_wtce");
  ui_vp_objection = new SplashLayer(this, ao_app);
  ui_vp_objection->set_play_once(true);
  ui_vp_objection->continuous = false;
  ui_vp_objection->force_continuous = true;
  ui_vp_objection->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_objection->setObjectName("ui_vp_objection");
}

bool AOViewport::isViewportBusy()
{
  return (m_text_state != TextState::DONE || m_text_stay_timer->isActive());
}
