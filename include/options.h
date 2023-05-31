#ifndef OPTIONS_H
#define OPTIONS_H


#include <QCoreApplication>
#include <QSettings>
#include <datatypes.h>

class Options {
private:
  /**
   * @brief QSettings object for config.ini
   */
  QSettings config;

  /**
   * @brief QSettings object for favorite_servers.ini
   */
  QSettings favorite;


  void migrateCallwords();

  /**
   * @brief Constructor for options class.
   */
  Options();

  QString m_server_subtheme;

public:
  Options(Options const &) = delete;
  void operator=(Options const &) = delete;

  static Options &getInstance()
  {
    static Options instance;
    return instance;
  }

  /**
   * @brief Migrates old configuration files to the most recent format.
   */
  void migrate();

  // Reads the theme from config.ini and loads it into the currenttheme
  // variable
  QString theme() const;
  void setTheme(QString value);

  // Returns the value of oocname in config.ini
  QString oocName() const;
  void setOocName(QString value);

  // Returns the blip rate from config.ini (once per X symbols)
  int blipRate() const;
  void setBlipRate(int value);

  // Returns true if blank blips is enabled in config.ini and false otherwise
  bool blankBlip() const;
  void setBlankBlip(bool value);

  // Returns true if looping sound effects are enabled in the config.ini
  bool loopingSfx() const;
  void setLoopingSfx(bool value);

  // Returns true if stop music on objection is enabled in the config.ini
  bool objectionStopMusic() const;
  void setObjectionStopMusic(bool value);

  // Returns true if streaming is enabled in the config.ini
  bool streamingEnabled() const;
  void setStreamingEnabled(bool value);

  // Returns the value of defaultmusic in config.ini
  int musicVolume() const;
  void setMusicVolume(int value);

  // Returns the value of defaultsfx in config.ini
  int sfxVolume() const;
  void setSfxVolume(int value);

  // Returns the value of defaultblip in config.ini
  int blipVolume() const;
  void setBlipVolume(int value);

  // Returns the value of suppressaudio in config.ini
  int defaultSuppressAudio() const;
  void setDefaultSupressedAudio(int value);

  // Returns the value if objections interrupt and skip the message queue
  // from the config.ini.
  bool objectionSkipQueueEnabled() const;
  void setObjectionSkipQueueEnabled(bool value);

  // returns if log will show messages as-received, while viewport will parse
  // according to the queue (Text Stay Time) from the config.ini
  bool desynchronisedLogsEnabled() const;
  void setDesynchronisedLogsEnabled(bool value);

  // Returns the value of whether Discord should be enabled on startup
  // from the config.ini.
  bool discordEnabled() const;
  void setDiscordEnabled(bool value);

  // Returns the value of whether shaking should be enabled.
  // from the config.ini.
  bool shakeEnabled() const;
  void setShakeEnabled(bool value);

  // Returns the value of whether effects should be Enabled.
  // from the config.ini.
  bool effectsEnabled() const;
  void setEffectsEnabled(bool value);

  // Returns the value of whether frame-specific effects defined in char.ini
  // should be sent/received over the network. from the config.ini.
  bool networkedFrameSfxEnabled() const;
  void setNetworkedFrameSfxEnabled(bool value);

  // Returns the value of whether colored ic log should be a thing.
  // from the config.ini.
  bool colorLogEnabled() const;
  void setColorLogEnabled(bool value);

  // Returns the value of whether sticky sounds should be a thing.
  // from the config.ini.
  bool clearSoundsDropdownOnPlayEnabled() const;
  void setClearSoundsDropdownOnPlayEnabled(bool value);

  // Returns the value of whether sticky effects should be a thing.
  // from the config.ini.
  bool clearEffectsDropdownOnPlayEnabled() const;
  void setClearEffectsDropdownOnPlayEnabled(bool value);

  // Returns the value of whether sticky preanims should be a thing.
  // from the config.ini.
  bool clearPreOnPlayEnabled() const;
  void setClearPreOnPlayEnabled(bool value);

  // Returns the value of whether custom chatboxes should be a thing.
  // from the config.ini.
  // I am increasingly maddened by the lack of dynamic auto-generation system
  // for settings.
  bool customChatboxEnabled() const;
  void setCustomChatboxEnabled(bool value);

  // Returns the value of characer sticker (avatar) setting
  bool characterStickerEnabled() const;
  void setCharacterStickerEnabled(bool value);

  // Returns the value of whether continuous playback should be used
  // from the config.ini.
  bool continuousPlaybackEnabled() const;
  void setContinuousPlaybackEnabled(bool value);

  // Returns the value of whether stopping music by double clicking category
  // should be used from the config.ini.
  bool stopMusicOnCategoryEnabled() const;
  void setStopMusicOnCategoryEnabled(bool value);

  // Returns the value of the maximum amount of lines the IC chatlog
  // may contain, from config.ini.
  int maxLogSize() const;
  void setMaxLogSize(int value);

  // Current wait time between messages for the queue system
  int textStayTime() const;
  void setTextStayTime(int value);

  // Returns the letter display speed during text crawl in in-character messages
  int textCrawlSpeed() const;
  void setTextCrawlSpeed(int value);

  // Returns Minimum amount of time (in miliseconds) that must pass before the
  // next Enter key press will send your IC message. (new behaviour)
  int chatRateLimit() const;
  void setChatRateLimit(int value);

  // Returns whether the log should go upwards (new behaviour)
  // or downwards (vanilla behaviour).
  bool logDirectionDownwards() const;
  void setLogDirectionDownwards(bool value);

  // Returns whether the log should separate name from text via newline or :
  bool logNewline() const;
  void setLogNewline(bool value);

  // Get spacing between IC log entries.
  int logMargin() const;
  void setLogMargin(int value);

  // Returns whether the log should have a timestamp.
  bool logTimestampEnabled() const;
  void setLogTimestampEnabled(bool value);

  // Returns the format string for the log timestamp
  QString logTimestampFormat() const;
  void setLogTimestampFormat(QString value);

  // Returns whether to log IC actions.
  bool logIcActions() const;
  void setLogIcActions(bool value);

  // Returns the username the user may have set in config.ini.
  QString username() const;
  void setUsername(QString value);

  // Returns the audio device used for the client.
  QString audioOutputDevice() const;
  void setAudioOutputDevice(QString value);

  // Returns whether the user would like to have custom shownames on by default.
  bool customShownameEnabled() const;
  void setCustomShownameEnabled(bool value);

  // Returns the showname the user may have set in config.ini.
  QString shownameOnJoin() const;
  void setShownameOnJoin(QString value);

  // Get if text file logging is Enabled
  bool logToTextFileEnabled() const;
  void setLogToTextFileEnabled(bool value);

  // Get if demo logging is Enabled
  bool logToDemoFileEnabled() const;
  void setLogToDemoFileEnabled(bool value);

  // Get the subtheme from settings
  QString subTheme() const;
  QString settingsSubTheme() const;
  void setSettingsSubTheme(QString value);

  // Returns the server-
  QString serverSubTheme() const;
  void setServerSubTheme(QString value);

  // Get if the theme is animated
  bool animatedThemeEnabled() const;
  void setAnimatedThemeEnabled(bool value);

  // Get the default scaling method
  QString defaultScalingMode() const;
  void setDefaultScalingMode(QString value);

  // Get a list of custom mount paths
  QStringList mountPaths() const;
  void setMountPaths(QStringList value);

  // Get whether to opt out of player count metrics sent to the master server
  bool playerCountOptout() const;
  void setPlayerCountOptout(bool value);

  // Get if sfx can be sent to play on idle
  bool playSelectedSFXOnIdle() const;
  void setPlaySelectedSFXOnIdle(bool value);

  // Whether opening evidence requires a single or double click
  bool evidenceDoubleClickEdit() const;
  void setEvidenceDoubleClickEdit(bool value);

  // Supplies an alternative masterserver URL
  QString alternativeMasterserver() const;
  void setAlternativeMasterserver(QString value);

  // Language the client loads on start.
  QString language() const;
  void setLanguage(QString value);

  // Callwords notify the user when the word/words are used in a game message.
  QStringList callwords() const;
  void setCallwords(QStringList value);

  // Clears the configuration file. Essentially restoring it to default.
  void clearConfig();

  // Loads the favorite servers
  QVector<server_type> favorites();
  void setFavorites(QVector<server_type> value);

  // Interactions with favorite servers
  void removeFavorite(int index);
  void addFavorite(server_type server);
  void updateFavorite(server_type server, int index);

  // Theming Nonesense!
  QString getUIAsset(QString f_asset_name);
};

#endif // OPTIONS_H
