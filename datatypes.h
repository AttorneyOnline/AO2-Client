#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>

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
};

struct chatmessage_type
{
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

struct area_type
{
  QString name;
  QString background;
  bool passworded;
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

#endif // DATATYPES_H
