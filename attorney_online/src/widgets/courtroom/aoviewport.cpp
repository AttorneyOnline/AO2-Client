#include "widgets/courtroom/aoviewport.h"
#include "options.h"

AOViewport::AOViewport(QWidget *p_parent, AOApplication* p_ao_app) :
    QWidget(p_parent), ao_app(p_ao_app)
{
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    Options options;

    ui_vp_background = new BackgroundLayer(this, ao_app);
    ui_vp_background->setObjectName("ui_vp_background");
    ui_vp_background->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_background->resize(options.viewportScale());

    ui_vp_speedlines = new SplashLayer(this, ao_app);
    ui_vp_speedlines->setObjectName("ui_vp_speedlines");
    ui_vp_speedlines->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_speedlines->resize(options.viewportScale());

    ui_vp_player_char = new CharLayer(this, ao_app);
    ui_vp_player_char->setObjectName("ui_vp_player_char");
    ui_vp_player_char->masked = false;
    ui_vp_player_char->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_player_char->resize(options.viewportScale());

    ui_vp_sideplayer_char = new CharLayer(this, ao_app);
    ui_vp_sideplayer_char->setObjectName("ui_vp_sideplayer_char");
    ui_vp_sideplayer_char->masked = false;
    ui_vp_sideplayer_char->hide();
    ui_vp_sideplayer_char->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_sideplayer_char->resize(options.viewportScale());

    ui_vp_desk = new BackgroundLayer(this, ao_app);
    ui_vp_desk->setObjectName("ui_vp_desk");
    ui_vp_desk->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_desk->resize(options.viewportScale());

    ui_vp_effect = new EffectLayer(this, ao_app);
    ui_vp_effect->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_vp_effect->setObjectName("ui_vp_effect");
    ui_vp_effect->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_effect->resize(options.viewportScale());

    ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);
    ui_vp_evidence_display->setObjectName("ui_vp_evidence_display");
    ui_vp_evidence_display->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_evidence_display->resize(options.viewportScale());

    ui_vp_sticker = new StickerLayer(this, ao_app);
    ui_vp_sticker->set_play_once(false);
    ui_vp_sticker->set_cull_image(false);
    ui_vp_sticker->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_vp_sticker->setObjectName("ui_vp_sticker");
    ui_vp_sticker->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_sticker->resize(options.viewportScale());

    ui_vp_chat_arrow = new InterfaceLayer(this, ao_app);
    ui_vp_chat_arrow->set_play_once(false);
    ui_vp_chat_arrow->set_cull_image(false);
    ui_vp_chat_arrow->setObjectName("ui_vp_chat_arrow");
    ui_vp_chat_arrow->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_chat_arrow->resize(options.viewportScale());

    ui_vp_testimony = new SplashLayer(this, ao_app);
    ui_vp_testimony->set_play_once(false);
    ui_vp_testimony->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_vp_testimony->setObjectName("ui_vp_testimony");
    ui_vp_testimony->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_testimony->resize(options.viewportScale());

    ui_vp_wtce = new SplashLayer(this, ao_app);
    ui_vp_wtce->set_play_once(true);
    ui_vp_wtce->continuous = false;
    ui_vp_wtce->force_continuous = true;
    ui_vp_wtce->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_vp_wtce->setObjectName("ui_vp_wtce");
    ui_vp_wtce->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_wtce->resize(options.viewportScale());

    ui_vp_objection = new SplashLayer(this, ao_app);
    ui_vp_objection->set_play_once(true);
    ui_vp_objection->continuous = false;
    ui_vp_objection->force_continuous = true;
    ui_vp_objection->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_vp_objection->setObjectName("ui_vp_objection");
    ui_vp_objection->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    ui_vp_objection->resize(options.viewportScale());
}
