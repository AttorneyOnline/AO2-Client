#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "datatypes.h"
#include "discord_rich_presence.h"
#include "options.h"

#include <QApplication>

#include <network/client.h>
#include <network/masterserver.h>

class Lobby;
class Courtroom;

class AOApplication : public QApplication
{
  Q_OBJECT

public:
  AOApplication(int &argc, char **argv);
  ~AOApplication() = default;

  std::shared_ptr<AttorneyOnline::Discord> discord;
  Options options;

  void openLobby();

  std::shared_ptr<AttorneyOnline::MasterServer> msConnection() { return ms; }

  //////////////////versioning///////////////

  constexpr int get_release() const { return RELEASE; }
  constexpr int get_major_version() const { return MAJOR_VERSION; }
  constexpr int get_minor_version() const { return MINOR_VERSION; }
  QString get_version_string();

  ///////////////////////////////////////////
  void reloadTheme();
  void connectToMaster();

  //implementation in path_functions.cpp
  QString get_base_path();
  QString get_data_path();
  QString get_theme_path(QString p_file);
  QString get_default_theme_path(QString p_file);
  QString get_custom_theme_path(QString p_theme, QString p_file);
  QString get_character_path(QString p_char, QString p_file);
  QString get_sounds_path(QString p_file);
  QString get_music_path(QString p_song);
  QString get_background_path(QString background, QString p_file);
  QString get_default_background_path(QString p_file);
  QString get_evidence_path(QString p_file);
  QString get_case_sensitive_path(QString p_file);

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  //Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, QString p_design_path);

  //Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file);

  //Returns the color with p_identifier from p_file
  QColor get_color(QString p_identifier, QString p_file);

  // Returns the colour from the misc folder.
  QColor get_chat_color(QString p_identifier, QString p_chat);

  //Returns the sfx with p_identifier from sounds.ini in the current theme path
  QString get_sfx(QString p_identifier);

  //Figure out if we can opus this or if we should fall back to wav
  QString get_sfx_suffix(QString sound_to_check);

  // Can we use APNG for this? If not, fall back to a gif.
  QString get_image_suffix(QString path_to_check);

  //Returns the value of p_search_line within target_tag and terminator_tag
  QString read_char_ini(QString p_char, QString p_search_line, QString target_tag);

  //Returns the side of the p_char character from that characters ini file
  QString get_char_side(QString p_char);

  //Returns the showname from the ini of p_char
  QString get_showname(QString p_char);

  //Returns the value of chat from the specific p_char's ini file
  QString get_chat(QString p_char);

  //Returns the value of shouts from the specified p_char's ini file
  QString get_char_shouts(QString p_char);

  //Returns the preanim duration of p_char's p_emote
  int get_preanim_duration(QString p_char, QString p_emote);

  //Same as above, but only returns if it has a % in front(refer to Preanims section in the manual)
  int get_ao2_preanim_duration(QString p_char, QString p_emote);

  //Not in use
  int get_text_delay(QString p_char, QString p_emote);

  // Returns the custom realisation used by the character.
  QString get_custom_realization(QString p_char);

  //Returns the name of p_char
  QString get_char_name(QString p_char);

  //Returns the total amount of emotes of p_char
  int get_emote_number(QString p_char);

  //Returns the emote comment of p_char's p_emote
  QString get_emote_comment(QString p_char, int p_emote);

  //Returns the base name of p_char's p_emote
  QString get_emote(QString p_char, int p_emote);

  //Returns the preanimation name of p_char's p_emote
  QString get_pre_emote(QString p_char, int p_emote);

  //Returns the sfx of p_char's p_emote
  QString get_sfx_name(QString p_char, int p_emote);

  //Not in use
  int get_sfx_delay(QString p_char, int p_emote);

  //Returns the modifier for p_char's p_emote
  EMOTE_MODIFIER get_emote_mod(QString p_char, int p_emote);

  //Returns the desk modifier for p_char's p_emote
  DESK_MODIFIER get_desk_mod(QString p_char, int p_emote);

  //Returns p_char's gender
  QString get_gender(QString p_char);

  // For debugging purposes only
  QString get_relative_path(QString path);
private:
  const int RELEASE = 2;
  const int MAJOR_VERSION = 6;
  const int MINOR_VERSION = 0;

  QString currentTheme = "default";

  const int MS_RETRY_MILLISECS = 4000;
  int msRetryCount = 0;
  std::shared_ptr<AttorneyOnline::MasterServer> ms;
};

#endif // AOAPPLICATION_H
