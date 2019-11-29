#ifndef AOVIEWPORT_H
#define AOVIEWPORT_H

#include "aoapplication.h"
#include "aoblipplayer.h"
#include "aocharmovie.h"
#include "aoevidencedisplay.h"
#include "aoimage.h"
#include "aomovie.h"
#include "aomusicplayer.h"
#include "aoscene.h"
#include "courtroom.h"

#include <QTextEdit>
#include <QWidget>

#include <stack>

class Courtroom;

class AOViewport : public QWidget
{
  Q_OBJECT
public:
  explicit AOViewport(QWidget *parent, AOApplication *p_ao_app);
  ~AOViewport();

  //sets the current background to argument. also does some checks to see if it's a legacy bg
  void set_background(QString p_background);
  QString get_current_background() { return current_background; }

  void chat(chat_message_type chatmessage, QString showname, QString evidence_image);
  bool is_busy() { return anim_state < 3 || text_state < 2; }

  //plays the witness testimony or cross examination animation based on argument
  void wtce(WTCE_TYPE type);

  void stop_sounds();
  void stop_talking();
private:
  chat_message_type m_chatmessage;
  QString showname;
  QString evidence_image;
  bool chatmessage_is_empty = false;

  // A stack of inline colours.
  std::stack<INLINE_COLOURS> inline_colour_stack;

  bool next_character_is_not_special = false; // If true, write the
                        // next character as it is.

  bool message_is_centered = false;

  int current_display_speed = 3;
  const int message_display_speed[7] = {30, 40, 50, 60, 75, 100, 120};

  // This is for checking if the character should start talking again
  // when an inline blue text ends.
  bool entire_message_is_blue = false;

  // And this is the inline 'talking checker'. Counts how 'deep' we are
  // in inline blues.
  int inline_blue_depth = 0;

  //determines how fast messages tick onto screen
  QTimer *chat_tick_timer;
  //int chat_tick_interval = 60;
  //which tick position(character in chat message) we are at
  int tick_pos = 0;
  //used to determine how often blips sound
  int blip_pos = 0;
  int blip_rate = 1;
  int rainbow_counter = 0;
  bool blank_blip = false;

  //delay before chat messages starts ticking
  QTimer *text_delay_timer;

  //delay before sfx plays
  QTimer *sfx_delay_timer;

  //keeps track of how long realization is visible(it's just a white square and should be visible less than a second)
  QTimer *realization_timer;

  //times how long the blinking testimony should be shown(green one in the corner)
  QTimer *testimony_show_timer;
  //times how long the blinking testimony should be hidden
  QTimer *testimony_hide_timer;

  //every time point in char.inis times this equals the final time
  const int time_mod = 40;

  bool testimony_in_progress = false;

  //in milliseconds
  const int testimony_show_time = 1500;

  //in milliseconds
  const int testimony_hide_time = 500;

  //state of animation, 0 = objecting, 1 = preanim, 2 = talking, 3 = idle, 4 = noniterrupting preanim
  int anim_state = 3;

  //state of text ticking, 0 = not yet ticking, 1 = ticking in progress, 2 = ticking done
  int text_state = 2;

  QString current_background = "default";

  //is set to true if the bg folder contains defensedesk.png, prosecutiondesk.png and stand.png
  bool is_ao2_bg = false;

  AOSfxPlayer *sfx_player;
  AOSfxPlayer *objection_player;
  AOBlipPlayer *blip_player;

  AOApplication *ao_app;
  Options options;

  AOScene *ui_vp_background;
  AOMovie *ui_vp_speedlines;

  AOCharMovie *ui_vp_player_char;
  AOCharMovie *ui_vp_sideplayer_char;

  AOScene *ui_vp_desk;
  AOScene *ui_vp_legacy_desk;

  AOEvidenceDisplay *ui_vp_evidence_display;

  AOImage *ui_vp_chatbox;
  QLabel *ui_vp_showname;
  QTextEdit *ui_vp_message;

  AOImage *ui_vp_testimony;
  AOImage *ui_vp_realization;
  AOMovie *ui_vp_wtce;
  AOMovie *ui_vp_objection;

  //these functions handle chatmessages sequentially.
  //The process itself is very convoluted and merits separate documentation
  //But the general idea is objection animation->pre animation->talking->idle
  void handle_chatmessage_2();
  void handle_chatmessage_3();

  void play_preanim(bool noninterrupting);

  QString get_background_path(QString file)
  {
    return ao_app->get_background_path(current_background, file);
  }

  //sets desk and bg based on pos in chatmessage
  void set_scene();

  //sets text color based on text color in chatmessage
  void set_text_color();

  // And gets the colour, too!
  QColor get_text_color(QString color);

signals:

public slots:
  void objection_done();
  void preanim_done();

  void realization_done();

  void show_testimony();
  void hide_testimony();

  void set_sfx_volume(int value);
  void set_blip_volume(int value);

private slots:
  void start_chat_ticking();
  void play_sfx();

  void chat_tick();
};

#endif // AOVIEWPORT_H
