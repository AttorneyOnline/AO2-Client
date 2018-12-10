#ifndef TEXT_FILE_FUNCTIONS_H
#define TEXT_FILE_FUNCTIONS_H

#include "aoapplication.h"
#include "file_functions.h"
#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QColor>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QRegExp>

/// A singleton designed to handle the config.ini file.
/// Call upon it with TextFileHandler::getInstance().
class TextFileHandler
{
  private:
    TextFileHandler() {}

    // Isn't this ironic? To eliminate pointers to the AO app,
    // I must still keep one.
    AOApplication *ao_app;

    QString current_theme = "default";

  public:
    static TextFileHandler& getInstance();
    TextFileHandler(TextFileHandler const&) = delete;
    void operator=(TextFileHandler const&) = delete;

    QSettings *configini;

    // Gives the pointer to the AO app to it.
    // Should be called right as the AO app itself is created.
    void give_ao_app_pointer(AOApplication *p_ao_app);

    // Gets the IP of the master server.
    QString get_master_server();

    //Reads the theme from config.ini and loads it into the current_theme variable
    void read_theme();

    // Since read_theme() now only manipulates this object only, in return, a new get_current_theme() function
    // is included here if you need the current theme for some reason.
    QString get_current_theme();

    //Returns the value of ooc_name in config.ini
    QString get_ooc_name();

    //Returns the blip rate from config.ini
    int read_blip_rate();

    //Returns true if blank blips is enabled in config.ini and false otherwise
    bool get_blank_blip();

    //Returns the value of default_music in config.ini
    int get_default_music();

    //Returns the value of default_sfx in config.ini
    int get_default_sfx();

    //Returns the value of default_blip in config.ini
    int get_default_blip();

    // Returns the value of whether Discord should be enabled on startup
    // from the config.ini.
    bool is_discord_enabled();

    // Returns the value of the maximum amount of lines the IC chatlog
    // may contain, from config.ini.
    int get_max_log_size();

    // Returns whether the log should go upwards (new behaviour)
    // or downwards (vanilla behaviour).
    bool get_log_goes_downwards();

    // Returns the username the user may have set in config.ini.
    QString get_default_username();

    // Returns the audio device used for the client.
    QString get_audio_output_device();

    // Returns whether the user would like to have custom shownames on by default.
    bool get_showname_enabled_by_default();

    //Returns the list of words in callwords.ini
    QStringList get_call_words();

    //Appends the argument string to serverlist.txt
    void write_to_serverlist_txt(QString p_line);

    //Returns the contents of serverlist.txt
    QVector<server_type> read_serverlist_txt();

    //Returns the value of p_identifier in the design.ini file in p_design_path
    QString read_design_ini(QString p_identifier, QString p_design_path);

    //Returns the coordinates of widget with p_identifier from p_file
    QPoint get_button_spacing(QString p_identifier, QString p_file);

    //Returns the dimensions of widget with specified identifier from p_file
    pos_size_type get_element_dimensions(QString p_identifier, QString p_file);

    //Returns the value of font_size with p_identifier from p_file
    int get_font_size(QString p_identifier, QString p_file);

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
    int get_emote_mod(QString p_char, int p_emote);

    //Returns the desk modifier for p_char's p_emote
    int get_desk_mod(QString p_char, int p_emote);

    //Returns p_char's gender
    QString get_gender(QString p_char);

    // Returns if the user has casing alerts enabled.
    bool get_casing_enabled();

    // Returns if the user wants to get alerts for the defence role.
    bool get_casing_defence_enabled();

    // Same for prosecution.
    bool get_casing_prosecution_enabled();

    // Same for judge.
    bool get_casing_judge_enabled();

    // Same for juror.
    bool get_casing_juror_enabled();

    // Same for steno.
    bool get_casing_steno_enabled();

    // Same for CM.
    bool get_casing_cm_enabled();

    // Get the message for the CM for casing alerts.
    QString get_casing_can_host_cases();

    // These below are all implemented in path_functions.cpp.

    QString get_base_path();
    QString get_data_path();
    QString get_theme_path(QString p_file);
    QString get_default_theme_path(QString p_file);
    QString get_custom_theme_path(QString p_theme, QString p_file);
    QString get_character_path(QString p_char, QString p_file);
    QString get_sounds_path(QString p_file);
    QString get_music_path(QString p_song);
    QString get_background_path(QString p_file);
    QString get_default_background_path(QString p_file);
    QString get_evidence_path(QString p_file);
    QString get_case_sensitive_path(QString p_file);
};

#endif // TEXT_FILE_FUNCTIONS_H
