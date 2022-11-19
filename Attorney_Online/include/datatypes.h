#ifndef DATATYPES_H
#define DATATYPES_H

#include <QMap>
#include <QString>
#include <QVariant>

namespace AttorneyOnline {

template <typename T> T toDataType(const int &f_int)
{
  return QVariant(f_int).value<T>();
}

template <typename T> T toDataType(const QString &f_string)
{
  return QVariant(f_string).value<T>();
}

class DataTypes {
  Q_GADGET

public:
  /**
   * @brief Controls when the "desk" image is shown in the viewport.
   *
   * <https://github.com/AttorneyOnline/docs/blob/master/docs/development/network.md#in-character-message>
   */
  enum class DESKMOD {
    CHAT = -1,
    HIDDEN = 0,
    SHOWN = 1,
    HIDEPREANIM = 2,
    SHOWPREANIM = 3,
    HIDEPREANIM_PAIR = 4,
    SHOWPREANIM_NOPAIR = 5
  };
  Q_ENUM(DESKMOD);

  /**
   * @brief Controls wether special emote effects are used.
   *
   * <https://github.com/AttorneyOnline/docs/blob/master/docs/development/network.md#in-character-message>
   */
  enum class EMOTE_MODIFIER {
    NONE = 0,
    PREANIM,
    OBJECTION,
    ZOOM = 5,
    OBJECT_ZOOM
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
    RAINBOW = 0
  };
  Q_ENUM(TEXT_COLOR);

  struct IncomingMSPacket {
    DESKMOD desk_mod = DESKMOD::HIDDEN;
    QString pre_animation = "";
    QString character ="Unknown";
    QString emote = "default";
    QString side = "wit";
    QString sfx_name = "";
    EMOTE_MODIFIER emote_mod = EMOTE_MODIFIER::NONE;
    int char_id = -1;
    int sfx_delay = 0;
    SHOUT_MODIFIER shout_mod = SHOUT_MODIFIER::NOTHING;
    QString evidence = "";
    bool flip = false;
    bool realization = false;
    TEXT_COLOR text_color = TEXT_COLOR::WHITE;
    //2.6 Network Extension.
    QString showname = character;
    int other_char_id = -1;
    QString other_char_name = "";
    QPair<int,int> character_offset = {0,0};
    QPair<int,int> other_offset = {0,0};
    bool other_flip = false;
    bool noninterruptpre = false;
    //2.8 Network Extension
    bool sfx_looping = false;
    bool screenshake = false;
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

    IncomingMSPacket(QStringList packet){
        Q_UNUSED(packet)
        desk_mod = toDataType<DESKMOD>(packet.at(DESK_MOD).toInt());
        pre_animation = packet.at(PRE_EMOTE);
        character = packet.at(CHAR_NAME);
        emote = packet.at(EMOTE);
        side = packet.at(SIDE);
        sfx_name = packet.at(SFX_NAME);
        emote_mod = toDataType<EMOTE_MODIFIER>(packet.at(EMOTE_MOD).toInt());
        char_id = packet.at(CHAR_ID).toInt();
        sfx_delay = packet.at(SFX_DELAY).toInt();
        shout_mod = toDataType<SHOUT_MODIFIER>(packet.at(OBJECTION_MOD));
        evidence = packet.at(EVIDENCE_ID).toInt();
    }

    QStringList serialise() {
        QStringList packet_content;
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
