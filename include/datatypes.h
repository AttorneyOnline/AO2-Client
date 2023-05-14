#ifndef DATATYPES_H
#define DATATYPES_H

#include <QMap>
#include <QString>

enum connection_type {
  TCP,
  WEBSOCKETS,
};

static QMap<QString, connection_type> to_connection_type = {
    {"tcp", connection_type::TCP},
    {"ws", connection_type::WEBSOCKETS}
};

struct server_type {
  QString name;
  QString desc;
  QString ip;
  int port;
  connection_type socket_type;
};

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
  QString background;
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

enum EMOTE_MOD_TYPE {
  IDLE = 0,
  PREANIM = 1,
  ZOOM = 5,
  PREANIM_ZOOM = 6,
};

enum DESK_MOD_TYPE {
  DESK_HIDE = 0,
  DESK_SHOW,
  DESK_EMOTE_ONLY,
  DESK_PRE_ONLY,
  DESK_EMOTE_ONLY_EX,
  DESK_PRE_ONLY_EX,
  //"EX" for "expanded"
  //dumb, i know, but throw the first stone if you have a better idea
};

enum MUSIC_EFFECT { FADE_IN = 1, FADE_OUT = 2, SYNC_POS = 4 };

#endif // DATATYPES_H
