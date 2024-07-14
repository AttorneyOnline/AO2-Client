#pragma once

#include <QMap>
#include <QString>

#include <optional>

struct CharacterSlot
{
  QString name;
  QString description;
  QString evidence_string;
  bool taken;
};

struct EvidenceItem
{
  QString name;
  QString description;
  QString image;
};

class BackgroundPosition
{
public:
  QString background;
  QString desk;
  std::optional<int> origin;
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
  IMMEDIATE,
  LOOPING_SFX,
  SCREENSHAKE,
  FRAME_SCREENSHAKE,
  FRAME_REALIZATION,
  FRAME_SFX,
  ADDITIVE,
  EFFECTS,
  BLIPNAME,
  SLIDE,
};

enum EMOTE_MOD_TYPE
{
  IDLE = 0,
  PREANIM = 1,
  ZOOM = 5,
  PREANIM_ZOOM = 6,
};

enum DESK_MOD_TYPE
{
  DESK_HIDE = 0,
  DESK_SHOW,
  DESK_EMOTE_ONLY,
  DESK_PRE_ONLY,
  DESK_EMOTE_ONLY_EX,
  DESK_PRE_ONLY_EX,
  //"EX" for "expanded"
  // dumb, i know, but throw the first stone if you have a better idea
};

enum MUSIC_EFFECT
{
  FADE_IN = 1,
  FADE_OUT = 2,
  SYNC_POS = 4
};

enum RESIZE_MODE
{
  AUTO_RESIZE_MODE,
  PIXEL_RESIZE_MODE,
  SMOOTH_RESIZE_MODE,
};

class PlayerData
{
public:
  int id = -1;
  QString name;
  QString character;
  QString character_name;
  int area_id = 0;
};

class PlayerRegister
{
public:
  enum REGISTER_TYPE
  {
    ADD_PLAYER,
    REMOVE_PLAYER,
  };

  int id;
  REGISTER_TYPE type;
};

class PlayerUpdate
{
public:
  enum DATA_TYPE
  {
    NAME,
    CHARACTER,
    CHARACTER_NAME,
    AREA_ID,
  };

  int id;
  DATA_TYPE type;
  QString data;
};
