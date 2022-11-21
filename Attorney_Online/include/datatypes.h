#ifndef DATATYPES_H
#define DATATYPES_H

#include "options.h"
#include <QMap>
#include <QString>
#include <QVariant>

template <typename T> T toDataType(const QString &f_string)
{
  return QVariant(f_string).value<T>();
}

template <typename T> QString fromDataType(const T &f_t)
{
  return QVariant::fromValue(f_t).toString();
}

namespace AttorneyOnline {

class DataTypes {
  Q_GADGET

public:
  /**
   * @brief Enumereration for all headers the client may receive from the
   * server.
   *
   * @details Header are the only way the client can differentiate incoming data
   * by type. If a header is not recogised by the client it will be discarded
   * without further processing.
   *
   * All headers provided here are for SERVER->CLIENT communication to eliminate
   * the ol reliable if/else chain.
   */
  enum class HEADER {
    ID,         //!< Software name and version
    PN,         //!< Playercount, maximum players and description
    FL,         //!< Feature list
    ASS,        //!< Asset URL
    SC,         //!< Character list
    LE,         //!< Evidence list
    SM,         //!< Musiclist, categories and areas
    DONE,       //!< Handshake finished confirmation
    CHARSCHECK, //!< Update taken characters
    PV,         //!< Selected character
    MS,         //!< IC-Message
    BN,         //!< Background
    MC,         //!< Playing song
    HP,         //!< Def/Pro health bar
    RT,         //!< WTCE-Animation
    SP,         //!< Position dropdown
    SD,         //!< Set position
    CT,         //!< OOC-Message
    ARUP,       //!< Area information - Status, Players, CMs and lock status
    FM,         //!< Musiclist only
    FA,         //!< Arealist only
    CASEA,      //!< Case Announcement
    AUTH,       //!< Authentication packet - controls guard button
    ZZ,         //!< Modcall
    KK,         //!< Kicked
    KB,         //!< Kicked and banned
    BD,         //!< Banned
    BB,         //!< Bessage Box
    CHECK,      //!< Keep-alive packet
    ST,         //!< Serverside subtheme selection
    TI,         //!< Timers
    JD          //!< Judge Controls
  };
  Q_ENUM(HEADER)

  /**
   * @brief Controls when the "desk" image is shown in the viewport.
   *
   * <https://github.com/AttorneyOnline/docs/blob/master/docs/development/network.md#in-character-message>
   */
  enum class DESKMOD {
    CHAT = -1,  //!< Mostly deprecated. Falls back to hardcoded table behaviour.
    HIDDEN = 0, //!< The desk is fully hidden.
    SHOWN = 1, //!< The desk is shown in front of character and other_character.
    HIDEPREANIM = 2, //!< The desk is hidden during preanim, shown when it ends
    SHOWPREANIM = 3, //!< The desk is shown during preanim, hidden when it ends
    HIDEPREANIM_PAIR = 4,  //!< The desk is hidden during preanim, character is
                           //!< centered and pairing is ignored, when it ends
                           //!< desk is shown and pairing is restored
    SHOWPREANIM_NOPAIR = 5 //!< The desk is shown during preanim, when it ends
                           //!< character is centered and pairing is ignored
  };
  Q_ENUM(DESKMOD)

  /**
   * @brief Controls wether special emote effects are used.
   *
   * <https://github.com/AttorneyOnline/docs/blob/master/docs/development/network.md#in-character-message>
   */
  enum class EMOTE_MODIFIER {
    NONE =
        0,   //!< No preanimation, Overridden to 2 by a non-0 objection modifier
    PREANIM, //!< Play preanimation and SFX.
    OBJECTION,  //!< Play objection, then preanimation.
    ZOOM = 5,   //!< No preanimation and apply zoom background.
    OBJECT_ZOOM //!< Play objection and apply zoom background after.
  };
  Q_ENUM(EMOTE_MODIFIER)

  /**
   * @brief Controls which shout is used.
   *
   * @details In typical AO fashion the custom one is bundled with the filename
   * in the same argument. Make sure to split the & delimiter away and only pass
   * the integer when checking for the enum.
   *
   * <https://github.com/AttorneyOnline/docs/blob/master/docs/development/network.md#in-character-message>
   */
  enum class SHOUT_MODIFIER {
    NOTHING = 0,
    HOLD_IT,
    OBJECTION,
    TAKE_THAT,
    CUSTOM
  };
  Q_ENUM(SHOUT_MODIFIER)

  /**
   * @brief Controls which color a text message appears in.
   *
   * @details Prior to 2.8 [CITATION NEEDED!] this is the only way to change the
   * color of your message, but it has been made largely obsolete by inline text
   * color, though still serves some purpose.
   *
   * Rainbow text has been remove in 2.8 and will just appear as plain white.
   * Sorry folks, no pride in AO.
   *
   */
  enum class TEXT_COLOR {
    WHITE = 0,
    GREEN,
    RED,
    ORANGE,
    BLUE,
    YELLOW,
    RAINBOW //!< DEPRECATED! Removed in 2.8!
  };
  Q_ENUM(TEXT_COLOR)

  struct MSPacket {
    DESKMOD desk_mod = DESKMOD::HIDDEN;
    QString pre_animation = "";
    QString character = "Unknown";
    QString emote = "default";
    QString message = "";
    QString side = "wit";
    QString sfx_name = "";
    EMOTE_MODIFIER emote_mod = EMOTE_MODIFIER::NONE;
    int char_id = -1;
    int sfx_delay = 0;
    SHOUT_MODIFIER shout_mod = SHOUT_MODIFIER::NOTHING;
    int evidence = 0;
    bool flip = false;
    bool realization = false;
    TEXT_COLOR text_color = TEXT_COLOR::WHITE;
    // 2.6 Network Extension.
    QString showname = character;
    int other_char_id = -1;
    QString other_char_name = "";
    QString other_char_emote = "";
    QPair<int, int> character_offset = {0, 0};
    QPair<int, int> other_offset = {0, 0};
    bool other_flip = false;
    bool immediate = false;
    // 2.8 Network Extension
    bool sfx_looping = false;
    bool screenshake = false;
    QStringList frame_fx_netstrings = QStringList{};
    bool additive = false;
    QString effect = "";

    enum indices {
      DESK_MOD = 0,
      PRE_EMOTE,
      CHAR_NAME,
      EMOTE,
      MESSAGE,
      SIDE,
      SFX_NAME,
      EMOTE_MOD,
      CHAR_ID,
      SFX_DELAY,
      OBJECTION_MOD,
      EVIDENCE_ID,
      FLIP,
      REALIZATION,
      COLOR,
      SHOWNAME,
      OTHER_CHARID,
      OTHER_NAME,
      OTHER_EMOTE,
      SELF_OFFSET,
      OTHER_OFFSET,
      OTHER_FLIP,
      IMMEDIATE,
      LOOPING_SFX,
      SCREENSHAKE,
      FRAME_SCREENSHAKE,
      FRAME_REALIZATION,
      FRAME_SFX,
      ADDITIVE,
      EFFECTS,
    };

    /**
     * @brief Constructor for an incoming IC-Chat packet
     *
     * @details Due to the way AO messages are encoded, there is no way to
     * safely determine what contents are in the packet without doing overly
     * many size checks.
     * So you get a KFO and CCCC packet extension check.
     * If you decide to not send your arguments correctly, that's on you.
     */
    MSPacket(QStringList packet, bool cccc_features, bool kfo_features)
    {
      desk_mod = toDataType<DESKMOD>(packet.at(DESK_MOD));
      pre_animation = packet.at(PRE_EMOTE);
      character = packet.at(CHAR_NAME);
      emote = packet.at(EMOTE);
      message = packet.at(MESSAGE);
      side = packet.at(SIDE);
      sfx_name = packet.at(SFX_NAME);
      emote_mod = toDataType<EMOTE_MODIFIER>(packet.at(EMOTE_MOD));
      char_id = packet.at(CHAR_ID).toInt();
      sfx_delay = packet.at(SFX_DELAY).toInt();
      shout_mod = toDataType<SHOUT_MODIFIER>(packet.at(OBJECTION_MOD));
      evidence = packet.at(EVIDENCE_ID).toInt();
      flip = toDataType<bool>(packet.at(FLIP));
      realization = toDataType<bool>(packet.at(REALIZATION));
      text_color = toDataType<TEXT_COLOR>(packet.at(COLOR));
      if (cccc_features) {
        showname = packet.at(SHOWNAME);
        other_char_id = packet.at(OTHER_CHARID).toInt();
        other_char_name = packet.at(OTHER_NAME);
        other_char_emote = packet.at(OTHER_EMOTE);

        QStringList offset = packet.at(SELF_OFFSET).split("&");
        if (offset.size() == 2) {
          character_offset = {offset[0].toInt(), offset[1].toInt()};
        }
        else {
          character_offset = {offset[0].toInt(), 0};
        }

        offset = packet.at(OTHER_OFFSET).split("&");
        if (offset.size() == 2) {
          other_offset = {offset[0].toInt(), offset[1].toInt()};
        }
        else {
          other_offset = {offset[0].toInt(), 0};
        }
        other_flip = toDataType<bool>(packet.at(OTHER_FLIP));
        immediate = toDataType<bool>(packet.at(IMMEDIATE));
      }
      if (kfo_features) {
        sfx_looping = toDataType<bool>(packet.at(LOOPING_SFX));
        screenshake = toDataType<bool>(packet.at(SCREENSHAKE));
        if (!packet.at(FRAME_SFX).isEmpty() &&
            Options::getInstance().networkedFrameSfxEnabled()) {
          QStringList netstrings = {packet.at(FRAME_SCREENSHAKE),
                                    packet.at(FRAME_REALIZATION),
                                    packet.at(FRAME_SFX)};
          frame_fx_netstrings = netstrings;
        }
        additive = toDataType<bool>(packet.at(ADDITIVE));
        effect = packet.at(EFFECTS);
      }
    }

    QStringList serialize()
    {
      QStringList packet_content;
      packet_content.append(fromDataType<DESKMOD>(desk_mod));
      packet_content.append(pre_animation);
      packet_content.append(character);
      packet_content.append(emote);
      packet_content.append(message);
      packet_content.append(side);
      packet_content.append(sfx_name);
      packet_content.append(fromDataType<EMOTE_MODIFIER>(emote_mod));
      packet_content.append(QString::number(char_id));
      packet_content.append(QString::number(sfx_delay));
      packet_content.append(fromDataType<SHOUT_MODIFIER>(shout_mod));
      packet_content.append(QString::number(evidence));
      return packet_content;
    }
  };

  struct IDPacket {
    int current_player;
    int max_players;
    QString server_description = "";

    IDPacket(const QStringList f_packet)
    {
      current_player = f_packet.at(0).toInt();
      max_players = f_packet.at(1).toInt();
      if (f_packet.size() >= 3) {
        server_description = f_packet.at(2);
      }
    };
  };

  struct STPacket {
    QString subtheme;
    bool reload_theme;

    STPacket(const QStringList f_packet)
    {
      subtheme = f_packet.at(0);
      if (f_packet.size() >= 2) reload_theme = toDataType<bool>(f_packet.at(1));
    }
  };

  enum class AUTHENTICATION {
    LOGOUT = -1,  //!< Log-out. Hides the guard button.
    LOGIN_FAIL,   //!< Unsuccessful log-in attempt.
    LOGIN_SUCCESS //!< Successful log-in. Displays the guard button.
  };
  Q_ENUM(AUTHENTICATION)

  /**
   * @brief The TIMER_ACTION enum
   *
   *  @details Allan, please add details.
   */
  enum class TIMER_ACTION {
    START, //!< Starts/Resumes/Syncs the timer given the time provided
    PAUSE, //!< Pauses the timer given the time provided
    SHOW,  //!< Shows timer UI element.
    HIDE   //!< Hides timer UI element.
  };
  Q_ENUM(TIMER_ACTION)

  struct TIPacket {
      int timer_id;
      TIMER_ACTION action;
      int timer_duration;

      TIPacket(QStringList f_packet) {
          timer_id = f_packet.at(0).toInt();
          action = toDataType<TIMER_ACTION>(f_packet.at(1));
          timer_duration = f_packet.at(2).toInt();
      };
  };

  struct CASEAPacket {
      QString message;
      bool need_def;
      bool need_pro;
      bool need_jud;
      bool need_jur;
      bool need_steno;

      CASEAPacket(QStringList f_packet) {
          message = f_packet.at(0);
          need_def = toDataType<bool>(f_packet.at(1));
          need_pro = toDataType<bool>(f_packet.at(2));
          need_jud = toDataType<bool>(f_packet.at(4));
          need_jur = toDataType<bool>(f_packet.at(5));
          need_steno = toDataType<bool>(f_packet.at(6));
      }

      QStringList serialize() {
          QStringList l_packet;
          l_packet << message;
          l_packet << QString::number(static_cast<int>(need_def));
          l_packet << QString::number(static_cast<int>(need_pro));
          l_packet << QString::number(static_cast<int>(need_jud));
          l_packet << QString::number(static_cast<int>(need_jur));
          l_packet << QString::number(static_cast<int>(need_steno));
          return l_packet;
      }
  };
};

} // namespace AttorneyOnline

//@{

/** Server Connection Shenanigans **/
enum connection_type {
  TCP,
  WEBSOCKETS,
};

static QMap<QString, connection_type> to_connection_type = {
    {"tcp", connection_type::TCP}, {"ws", connection_type::WEBSOCKETS}};

struct server_type {
  QString name;
  QString desc;
  QString ip;
  int port;
  connection_type socket_type;
};
//@}

struct emote_type {
  QString comment;
  QString preanim;
  QString anim;
  int mod;
  QString sfx_name;
  int sfx_delay;
  int sfx_duration;
};

struct char_type {
  QString name;
  QString description;
  QString evidence_string;
  bool taken;
};

struct evi_type {
  QString name;
  QString description;
  QString image;

  operator QStringList() const { return {name, description, image}; }
};

struct chatmessage_type {
  QString message;
  QString character;
  QString side;
  QString sfx_name;
  QString pre_emote;
  QString emote;
  int emote_modifier;
  int objection_modifier;
  int realization;
  int text_color;
  int evidence;
  int cid;
  int sfx_delay;
  int flip;
};

struct area_type {
  QString name;
  int id;
  int players;
  QString background;
  QString status;
  QString cm;
  QString locked;
  bool passworded;
};

struct pos_type {
  int x;
  int y;
};

struct pos_size_type {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
};

enum CHAT_MESSAGE {
  DESK_MOD = 0,
  PRE_EMOTE,
  CHAR_NAME,
  EMOTE,
  MESSAGE,
  SIDE,
  SFX_NAME,
  EMOTE_MOD,
  CHAR_ID,
  SFX_DELAY,
  OBJECTION_MOD,
  EVIDENCE_ID,
  FLIP,
  REALIZATION,
  TEXT_COLOR,
  SHOWNAME,
  OTHER_CHARID,
  OTHER_NAME,
  OTHER_EMOTE,
  SELF_OFFSET,
  OTHER_OFFSET,
  OTHER_FLIP,
  IMMEDIATE,
  LOOPING_SFX,
  SCREENSHAKE,
  FRAME_SCREENSHAKE,
  FRAME_REALIZATION,
  FRAME_SFX,
  ADDITIVE,
  EFFECTS,
};

enum EMOTE_MOD {
  IDLE = 0,
  PREANIM = 1,
  ZOOM = 5,
  PREANIM_ZOOM = 6,
};

enum COLOR { WHITE = 0, GREEN, RED, ORANGE, BLUE, YELLOW, RAINBOW, PINK, CYAN };

// This is for inline message-colouring.
enum INLINE_COLOURS { INLINE_BLUE, INLINE_GREEN, INLINE_ORANGE, INLINE_GREY };

enum ARUP_PARAM { PLAYERS = 0, STATUS, CM, LOCKED };

enum HEALTH_TYPE { DEF = 1, PRO };

enum WTCE_TYPE { WITNESS_TESTIMONY, CROSS_EXAMINATION, NOT_GUILTY, GUILTY };

enum AUDIO_TYPE { MUSIC, SFX, BLIPS };

enum MUSIC_EFFECT { FADE_IN = 1, FADE_OUT = 2, SYNC_POS = 4 };

#endif // DATATYPES_H
