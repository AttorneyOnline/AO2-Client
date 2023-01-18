#include "widgets/courtroom/aoviewport.h"
#include "options.h"

AOViewport::AOViewport(QWidget *p_parent, AOApplication *p_ao_app)
    : QWidget(p_parent), ao_app(p_ao_app)
{
  setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  Options options;

  ui_vp_background = new BackgroundLayer(this, ao_app);
  ui_vp_background->setObjectName("ui_vp_background");
  ui_vp_background->comboResize(options.viewportScale());

  ui_vp_speedlines = new SplashLayer(this, ao_app);
  ui_vp_speedlines->setObjectName("ui_vp_speedlines");
  ui_vp_speedlines->comboResize(options.viewportScale());

  ui_vp_player_char = new CharLayer(this, ao_app);
  ui_vp_player_char->setObjectName("ui_vp_player_char");
  ui_vp_player_char->masked = false;
  ui_vp_player_char->comboResize(options.viewportScale());

  ui_vp_sideplayer_char = new CharLayer(this, ao_app);
  ui_vp_sideplayer_char->setObjectName("ui_vp_sideplayer_char");
  ui_vp_sideplayer_char->masked = false;
  ui_vp_sideplayer_char->hide();
  ui_vp_sideplayer_char->comboResize(options.viewportScale());

  ui_vp_desk = new BackgroundLayer(this, ao_app);
  ui_vp_desk->setObjectName("ui_vp_desk");
  ui_vp_desk->comboResize(options.viewportScale());

  ui_vp_effect = new EffectLayer(this, ao_app);
  ui_vp_effect->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_effect->setObjectName("ui_vp_effect");
  ui_vp_effect->comboResize(options.viewportScale());

  ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);
  ui_vp_evidence_display->setObjectName("ui_vp_evidence_display");
  ui_vp_evidence_display->resize(options.viewportScale());

  ui_vp_sticker = new StickerLayer(this, ao_app);
  ui_vp_sticker->set_play_once(false);
  ui_vp_sticker->set_cull_image(false);
  ui_vp_sticker->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_sticker->setObjectName("ui_vp_sticker");
  ui_vp_sticker->comboResize(options.viewportScale());

  ui_vp_chat_arrow = new InterfaceLayer(this, ao_app);
  ui_vp_chat_arrow->set_play_once(false);
  ui_vp_chat_arrow->set_cull_image(false);
  ui_vp_chat_arrow->setObjectName("ui_vp_chat_arrow");
  ui_vp_chat_arrow->comboResize(options.viewportScale());

  ui_vp_testimony = new SplashLayer(this, ao_app);
  ui_vp_testimony->set_play_once(false);
  ui_vp_testimony->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_testimony->setObjectName("ui_vp_testimony");
  ui_vp_testimony->comboResize(options.viewportScale());

  ui_vp_wtce = new SplashLayer(this, ao_app);
  ui_vp_wtce->set_play_once(true);
  ui_vp_wtce->continuous = false;
  ui_vp_wtce->force_continuous = true;
  ui_vp_wtce->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_wtce->setObjectName("ui_vp_wtce");
  ui_vp_wtce->comboResize(options.viewportScale());

  ui_vp_objection = new SplashLayer(this, ao_app);
  ui_vp_objection->set_play_once(true);
  ui_vp_objection->continuous = false;
  ui_vp_objection->force_continuous = true;
  ui_vp_objection->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_objection->setObjectName("ui_vp_objection");
  ui_vp_objection->comboResize(options.viewportScale());

  m_text_stay.setSingleShot(true);
  m_text_stay.setInterval(options.textStayTime());
}

void AOViewport::setEffectLayerPosition(QString effect_layer_option)
{
  QMap<QString, AOLayer *> layer_map{{"behind", ui_vp_player_char},
                                     {"character", ui_vp_player_char},
                                     {"over", ui_vp_player_char},
                                     {"chat", ui_vp_player_char}};
  ui_vp_effect->stackUnder(layer_map.value(effect_layer_option));
}

bool AOViewport::isBusy()
{
  return anim_state != AnimationState::IDLE ||
         text_state != TextState::IDLE || m_text_stay.isActive();
}

void AOViewport::startChat(DataTypes::MSPacket f_packet)
{
  Q_UNUSED(f_packet)
    setScene(f_packet.desk_mod, f_packet.side);

}

QTimer *AOViewport::getTextStayTimer() { return &m_text_stay; }

void AOViewport::ObjectionInterrupt()
{
    // The queue is being skipped!
}

void AOViewport::onBackgroundChanged(QString f_background)
{
    m_current_background = f_background;
    ui_vp_background->background = m_current_background;

    if(!isBusy()) {
        ui_vp_background->loadImage();
    }
}

void AOViewport::onBackgroundChanged(QString f_background, QString f_side)
{
    m_current_background = f_background;
    ui_vp_background->background = m_current_background;
    ui_vp_background->side = f_side;

    if(!isBusy()) {
        ui_vp_background->loadImage();
    }
}

void AOViewport::setScene(DataTypes::DESKMOD f_deskmod, QString f_side)
{
    ui_vp_background->side = f_side;
    ui_vp_background->loadImage();
    ui_vp_desk->load_image(ao_app->get_pos_path(f_side, true));

    if (f_deskmod == DataTypes::DESKMOD::HIDDEN ||
        (f_deskmod != DataTypes::DESKMOD::SHOWN &&
         (f_side == "jud" || f_side == "hld" || f_side == "hlp"))) {
      ui_vp_desk->hide();
    }
    else {
      ui_vp_desk->show();
    }
}
