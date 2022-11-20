#ifndef DATATYPES_H
#define DATATYPES_H

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
  Q_ENUM(DESKMOD);

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
  Q_ENUM(EMOTE_MODIFIER);

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
  Q_ENUM(SHOUT_MODIFIER);

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
  Q_ENUM(TEXT_COLOR);

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
    QVector<int> frames_shake = QVector<int>{};
    QVector<int> frames_realization = QVector<int>{};
    QVector<int> frames_sfx = QVector<int>{};
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

    MSPacket(QStringList packet)
    {
      Q_UNUSED(packet)
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
      sfx_looping = toDataType<bool>(packet.at(LOOPING_SFX));
      screenshake = toDataType<bool>(packet.at(SCREENSHAKE));
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
