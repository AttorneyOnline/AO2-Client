#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <bitset>

struct server_type
{
  QString name;
  QString desc;
  QString ip;
  int port;
};

struct emote_type
{
  QString comment;
  QString preanim;
  QString anim;
  int mod;
  QString sfx_name;
  int sfx_delay;
  int sfx_duration;
};

struct char_type
{
  QString name;
  QString description;
  QString evidence_string;
  bool taken;
};

struct evi_type
{
  QString name;
  QString description;
  QString image;

  operator QStringList() const
  {
    return {name, description, image};
  }
};

struct area_type
{
  QString name;
  int id;
  int players;
  QString status;
  QString cm;
  QString locked;
};

struct track_type
{
  QString name;
  int id;
};

struct pos_type
{
  int x;
  int y;
};

struct pos_size_type
{
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
};

enum CHAT_MESSAGE
{
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
  NONINTERRUPTING_PRE
};

enum COLOR
{
  WHITE = 0,
  GREEN,
  RED,
  ORANGE,
  BLUE,
  YELLOW,
  RAINBOW,
  PINK,
  CYAN
};

// This is for inline message-colouring.
enum INLINE_COLOURS {
  INLINE_BLUE,
  INLINE_GREEN,
  INLINE_ORANGE,
  INLINE_GREY
};

enum ARUP_PARAM
{
  PLAYERS = 0,
  STATUS,
  CM,
  LOCKED
};

enum HEALTH_TYPE
{
  DEF = 1,
  PRO
};

enum WTCE_TYPE
{
  WITNESS_TESTIMONY,
  CROSS_EXAMINATION,
  NOT_GUILTY,
  GUILTY
};

enum AUDIO_TYPE
{
  MUSIC,
  SFX,
  BLIPS
};

enum DESK_MODIFIER
{
  DEFAULT_DESK_MOD,
  NO_DESK,
  ALWAYS_DESK
};

enum EMOTE_MODIFIER
{
  NO_PREANIM = 0,
  PREANIM,
  OBJECTION_PREANIM,
  ZOOM = 5,
  OBJECTION_ZOOM
};

enum OBJECTION_TYPE
{
  NONE = 0,
  HOLD_IT,
  OBJECTION,
  TAKE_THAT,
  CUSTOM
};

enum CASING_FLAGS
{
  CASING_DEF = 0,
  CASING_PRO,
  CASING_JUD,
  CASING_JUR,
  CASING_STENO,
  CASING_CM
};
const auto CASING_FLAGS_COUNT = 6;

std::bitset<CASING_FLAGS_COUNT>
inline casing_flags_to_bitset(bool def, bool pro, bool jud, bool jur,
                              bool steno, bool cm)
{
  std::bitset<CASING_FLAGS_COUNT> bitset;
  bitset[CASING_DEF] = def;
  bitset[CASING_PRO] = pro;
  bitset[CASING_JUD] = jud;
  bitset[CASING_JUR] = jur;
  bitset[CASING_STENO] = steno;
  bitset[CASING_CM] = cm;
  return bitset;
}

struct chat_message_type
{
  DESK_MODIFIER desk;
  QString preanim;
  QString character;
  QString anim;
  QString message;
  QString side;
  QString sfx_name;
  EMOTE_MODIFIER emote_modifier;
  int char_id;
  int sfx_delay;
  OBJECTION_TYPE objection_modifier;
  int evidence = 0; // 0 if no evidence (one-indexed)
  bool flip;
  bool realization;
  COLOR text_color;
  QString showname = "";
  int pair_char_id = -1;
  int self_offset = 0;
  bool noninterrupting_preanim = false;

  QString pair_character;
  QString pair_anim;
  int pair_offset;
  bool pair_flip;

  chat_message_type() {}

  chat_message_type(const QStringList &contents)
  {
    desk = contents[0] == "chat" ? DEFAULT_DESK_MOD
                                 : static_cast<DESK_MODIFIER>(contents[0].toInt());
    preanim = contents[1];
    character = contents[2];
    anim = contents[3];
    message = contents[4];
    side = contents[5];
    sfx_name = contents[6];
    emote_modifier = static_cast<EMOTE_MODIFIER>(contents[7].toInt());
    char_id = contents[8].toInt();
    sfx_delay = contents[9].toInt();
    objection_modifier = static_cast<OBJECTION_TYPE>(contents[10].toInt());
    evidence = contents[11].toInt();
    flip = contents[12].toInt();
    realization = contents[13].toInt();
    text_color = static_cast<COLOR>(contents[14].toInt());

    if (contents.size() < 16)
      return;

    showname = contents[15];

    // pair_char_id is basically unused. Stupid...
    bool ok;
    pair_char_id = contents[16].toInt(&ok);
    if (!ok)
      pair_char_id = -1;

    pair_character = contents[17];
    pair_anim = contents[18];
    self_offset = contents[19].toInt();
    pair_offset = contents[20].toInt();
    pair_flip = contents[21].toInt();
    noninterrupting_preanim = contents[22].toInt();
  }

  QStringList serialize(bool extended) const
  {
    QStringList list = {
      desk == 0 ? "chat" : QString::number(desk),
      preanim,
      character,
      anim,
      message,
      side,
      sfx_name,
      QString::number(emote_modifier),
      QString::number(char_id),
      QString::number(sfx_delay),
      QString::number(objection_modifier),
      QString::number(evidence),
      QString::number(flip),
      QString::number(realization),
      QString::number(text_color),
    };

    if (extended)
    {
      list.append({
        showname,
        QString::number(pair_char_id),
        QString::number(pair_offset),
        QString::number(noninterrupting_preanim)
      });
    }

    return list;
  }
};

#endif // DATATYPES_H
