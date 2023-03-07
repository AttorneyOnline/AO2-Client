#include "options.h"
#include "file_functions.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QObject>
#include <QRegularExpression>
#include <QSize>

void Options::migrateCallwords()
{
  // Bla bla, evil boilerplate.
  QStringList l_callwords;

  QFile l_file;
  l_file.setFileName(get_base_path() + "callwords.ini");

  if (!l_file.open(QIODevice::ReadOnly)) {
    qWarning() << "Unable to migrate callwords : File not open.";
    return;
  }

  QTextStream in(&l_file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  in.setCodec("UTF-8");
#endif

  while (!in.atEnd()) {
    QString line = in.readLine();
    l_callwords.append(line);
  }
  l_file.close();
  l_file.remove();

  setCallwords(l_callwords);
}

Options::Options()
    : config(get_base_path() + "config.ini", QSettings::IniFormat, nullptr),
      favorite(get_base_path() + "favorite_servers.ini", QSettings::IniFormat,
               nullptr)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  config.setIniCodec("UTF-8");
#endif
  migrate();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  favorite.setIniCodec("UTF-8");
#endif
}

/*! Migrate old configuration keys/values to a relevant format. */
void Options::migrate()
{
  if (config.contains("show_custom_shownames")) {
    config.remove("show_custom_shownames");
  }
  if (QFile::exists(get_base_path() + "callwords.ini")) {
    migrateCallwords();
  }
  if (config.contains("ooc_name")) {
    if (username().isEmpty()) {
      config.setValue("default_username", config.value("ooc_name"));
    }
    config.remove("ooc_name");
  }

  if (config.contains("casing_enabled")) {
    config.remove("casing_enabled");
    config.remove("casing_defence_enabled");
    config.remove("casing_prosecution_enabled");
    config.remove("casing_judge_enabled");
    config.remove("casing_juror_enabled");
    config.remove("casing_steno_enabled");
    config.remove("casing_cm_enabled");
    config.remove("casing_can_host_cases");
  }
}

QString Options::theme() const
{
  return config.value("theme", "default").toString();
}

void Options::setTheme(QString value) { config.setValue("theme", value); }

int Options::blipRate() const { return config.value("blip_rate", 2).toInt(); }

void Options::setBlipRate(int value) { config.setValue("blip_rate", value); }

int Options::musicVolume() const
{
  return config.value("default_music", 50).toInt();
}

void Options::setMusicVolume(int value)
{
  config.setValue("default_music", value);
}

int Options::sfxVolume() const
{
  return config.value("default_sfx", 50).toInt();
}

void Options::setSfxVolume(int value) { config.setValue("default_sfx", value); }

int Options::blipVolume() const
{
  return config.value("default_blip", 50).toInt();
}

void Options::setBlipVolume(int value)
{
  config.setValue("default_blip", value);
}

int Options::defaultSuppressAudio() const
{
  return config.value("suppress_audio", 50).toInt();
}

void Options::setDefaultSupressedAudio(int value)
{
  config.setValue("suppress_audio", value);
}

int Options::maxLogSize() const
{
  return config.value("log_maximum", 200).toInt();
}

void Options::setMaxLogSize(int value)
{
  config.setValue("log_maximum", value);
}

int Options::textStayTime() const
{
  return config.value("stay_time", 200).toInt();
}

void Options::setTextStayTime(int value)
{
  config.setValue("stay_time", value);
}

int Options::textCrawlSpeed() const
{
  return config.value("text_crawl", 40).toInt();
}

void Options::setTextCrawlSpeed(int value)
{
  config.setValue("text_crawl", value);
}

int Options::chatRateLimit() const
{
  return config.value("chat_ratelimit", 300).toInt();
}

void Options::setChatRateLimit(int value)
{
  config.setValue("chat_ratelimit", value);
}

bool Options::logDirectionDownwards() const
{
  return config.value("log_goes_downwards", true).toBool();
}

void Options::setLogDirectionDownwards(bool value)
{
  config.setValue("log_goes_downwards", value);
}

bool Options::logNewline() const
{
  return config.value("log_newline", false).toBool();
}

void Options::setLogNewline(bool value)
{
  config.setValue("log_newline", value);
}

int Options::logMargin() const { return config.value("log_margin", 0).toInt(); }

void Options::setLogMargin(int value) { config.setValue("log_margin", value); }

bool Options::logTimestampEnabled() const
{
  return config.value("log_timestamp", false).toBool();
}

void Options::setLogTimestampEnabled(bool value)
{
  config.setValue("log_timestamp", value);
}

QString Options::logTimestampFormat() const
{
  return config.value("log_timestamp_format", "h:mm:ss AP").toString();
}

void Options::setLogTimestampFormat(QString value)
{
  config.setValue("log_timestamp_format", value);
}

bool Options::logIcActions() const
{
  return config.value("log_ic_actions", true).toBool();
}

void Options::setLogIcActions(bool value)
{
  config.setValue("log_ic_actions", value);
}

bool Options::customShownameEnabled() const
{
  return config.value("show_custom_shownames", true).toBool();
}

void Options::setCustomShownameEnabled(bool value)
{
  config.setValue("show_custom_shownames", value);
}

QString Options::username() const
{
  return config.value("default_username", "").value<QString>();
}

void Options::setUsername(QString value)
{
  config.setValue("default_username", value);
}

QString Options::shownameOnJoin() const
{
  return config.value("default_showname", "").toString();
}

void Options::setShownameOnJoin(QString value)
{
  config.setValue("default_showname", value);
}

QString Options::audioOutputDevice() const
{
  return config.value("default_audio_device", "default").toString();
}

void Options::setAudioOutputDevice(QString value)
{
  config.setValue("default_audio_device", value);
}

bool Options::blankBlip() const
{
  return config.value("blank_blip", false).toBool();
}

void Options::setBlankBlip(bool value) { config.setValue("blank_blip", value); }

bool Options::loopingSfx() const
{
  return config.value("looping_sfx", true).toBool();
}

void Options::setLoopingSfx(bool value)
{
  config.setValue("looping_sfx", value);
}

bool Options::objectionStopMusic() const
{
  return config.value("objection_stop_music", false).toBool();
}

void Options::setObjectionStopMusic(bool value)
{
  config.setValue("objection_stop_music", value);
}

bool Options::streamingEnabled() const
{
  return config.value("streaming_enabled", true).toBool();
}

void Options::setStreamingEnabled(bool value)
{
  config.setValue("streaming_enabled", value);
}

bool Options::objectionSkipQueueEnabled() const
{
  return config.value("instant_objection", true).toBool();
}

void Options::setObjectionSkipQueueEnabled(bool value)
{
  config.setValue("instant_objection", value);
}

bool Options::desynchronisedLogsEnabled() const
{
  return config.value("desync_logs", false).toBool();
}

void Options::setDesynchronisedLogsEnabled(bool value)
{
  config.setValue("desync_logs", value);
}

bool Options::discordEnabled() const
{
  return config.value("discord", true).toBool();
}

void Options::setDiscordEnabled(bool value)
{
  config.setValue("discord", value);
}

bool Options::shakeEnabled() const
{
  return config.value("shake", true).toBool();
}

void Options::setShakeEnabled(bool value) { config.setValue("shake", value); }

bool Options::effectsEnabled() const
{
  return config.value("effects", true).toBool();
}

void Options::setEffectsEnabled(bool value)
{
  config.setValue("effects", value);
}

bool Options::networkedFrameSfxEnabled() const
{
  return config.value("framenetwork", true).toBool();
}

void Options::setNetworkedFrameSfxEnabled(bool value)
{
  config.setValue("framenetwork", value);
}

bool Options::colorLogEnabled() const
{
  return config.value("colorlog", true).toBool();
}

void Options::setColorLogEnabled(bool value)
{
  config.setValue("colorlog", value);
}

bool Options::clearSoundsDropdownOnPlayEnabled() const
{
  return config.value("stickysounds", true).toBool();
}

void Options::setClearSoundsDropdownOnPlayEnabled(bool value)
{
  config.setValue("stickysounds", value);
}

bool Options::clearEffectsDropdownOnPlayEnabled() const
{
  return config.value("stickyeffects", true).toBool();
}

void Options::setClearEffectsDropdownOnPlayEnabled(bool value)
{
  config.setValue("stickyeffects", value);
}

bool Options::clearPreOnPlayEnabled() const
{
  return config.value("stickypres", true).toBool();
}

void Options::setClearPreOnPlayEnabled(bool value)
{
  config.setValue("stickypres", value);
}

bool Options::customChatboxEnabled() const
{
  return config.value("customchat", true).toBool();
}

void Options::setCustomChatboxEnabled(bool value)
{
  config.setValue("customchat", value);
}

bool Options::characterStickerEnabled() const
{
  return config.value("sticker", true).toBool();
}

void Options::setCharacterStickerEnabled(bool value)
{
  config.setValue("sticker", value);
}

bool Options::continuousPlaybackEnabled() const
{
  return config.value("continuous_playback", true).toBool();
}

void Options::setContinuousPlaybackEnabled(bool value)
{
  config.setValue("continuous_playback", value);
}

bool Options::stopMusicOnCategoryEnabled() const
{
  return config.value("category_stop", true).toBool();
}

void Options::setStopMusicOnCategoryEnabled(bool value)
{
  config.setValue("category_stop", value);
}

bool Options::logToTextFileEnabled() const
{
  return config.value("automatic_logging_enabled", true).toBool();
}

void Options::setLogToTextFileEnabled(bool value)
{
  config.setValue("automatic_logging_enabled", value);
}

bool Options::logToDemoFileEnabled() const
{
  return config.value("demo_logging_enabled", true).toBool();
}

void Options::setLogToDemoFileEnabled(bool value)
{
  config.setValue("demo_logging_enabled", value);
}

QString Options::subTheme() const
{
  if (settingsSubTheme() == "server" && !m_server_subtheme.isEmpty()) {
    return m_server_subtheme;
  }
  return settingsSubTheme();
}

QString Options::settingsSubTheme() const
{
  return config.value("subtheme", "server").toString();
}

void Options::setSettingsSubTheme(QString value)
{
  config.setValue("subtheme", value);
}

QString Options::serverSubTheme() const { return m_server_subtheme; }

void Options::setServerSubTheme(QString value) { m_server_subtheme = value; }

bool Options::animatedThemeEnabled() const
{
  return config.value("animated_theme", true).toBool();
}

void Options::setAnimatedThemeEnabled(bool value)
{
  config.setValue("animated_theme", value);
}

QString Options::defaultScalingMode() const
{
  return config.value("default_scaling", "fast").toString();
}

void Options::setDefaultScalingMode(QString value)
{
  config.setValue("default_scaling", value);
}

QStringList Options::mountPaths() const
{
  return config.value("mount_paths").value<QStringList>();
}

void Options::setMountPaths(QStringList value)
{
  config.setValue("mount_paths", value);
}

bool Options::playerCountOptout() const
{
  return config.value("player_count_optout", false).toBool();
}

void Options::setPlayerCountOptout(bool value)
{
  config.setValue("player_count_optout", value);
}

bool Options::playSelectedSFXOnIdle() const
{
  return config.value("sfx_on_idle", false).toBool();
}

void Options::setPlaySelectedSFXOnIdle(bool value)
{
  config.setValue("sfx_on_idle", value);
}

bool Options::evidenceDoubleClickEdit() const
{
  return config.value("evidence_double_click", true).toBool();
}

void Options::setEvidenceDoubleClickEdit(bool value)
{
  config.setValue("evidence_double_click", value);
}

QString Options::alternativeMasterserver() const
{
  return config.value("master", "").toString();
}

void Options::setAlternativeMasterserver(QString value)
{
  config.setValue("master", value);
}

QString Options::language() const
{
  return config.value("language", QLocale::system().name()).toString();
}

void Options::setLanguage(QString value) { config.setValue("language", value); }

QStringList Options::callwords() const
{
  QStringList l_callwords =
      config.value("callwords", QStringList{}).toStringList();

  // Please someone explain to me how tf I am supposed to create an empty
  // QStringList using QSetting defaults.
  if (l_callwords.size() == 1 && l_callwords.at(0).isEmpty())
    l_callwords.clear();
  return l_callwords;
}

void Options::setCallwords(QStringList value)
{
  config.setValue("callwords", value);
}

void Options::clearConfig() { config.clear(); }

QVector<server_type> Options::favorites()
{
  QVector<server_type> serverlist;

  auto grouplist = favorite.childGroups();
  { // remove all negative and non-numbers
    auto filtered_grouplist = grouplist;
    for (const QString &group : qAsConst(grouplist)) {
      bool ok = false;
      const int l_num = group.toInt(&ok);
      if (ok && l_num >= 0) {
        continue;
      }
      filtered_grouplist.append(group);
    }
    std::sort(filtered_grouplist.begin(), filtered_grouplist.end(),
              [](const auto &a, const auto &b) -> bool {
                return a.toInt() < b.toInt();
              });
    grouplist = std::move(filtered_grouplist);
  }

  for (const QString &group : qAsConst(grouplist)) {
    server_type f_server;
    favorite.beginGroup(group);
    f_server.ip = favorite.value("address", "127.0.0.1").toString();
    f_server.port = favorite.value("port", 27016).toInt();
    f_server.name = favorite.value("name", "Missing Name").toString();
    f_server.desc = favorite.value("desc", "No description").toString();
    f_server.socket_type =
        to_connection_type.value(favorite.value("protocol", "tcp").toString());
    serverlist.append(std::move(f_server));
    favorite.endGroup();
  }

  return serverlist;
}

void Options::setFavorites(QVector<server_type> value)
{
  favorite.clear();
  for (int i = 0; i < value.size(); ++i) {
    auto fav_server = value.at(i);
    favorite.beginGroup(QString::number(i));
    favorite.setValue("name", fav_server.name);
    favorite.setValue("address", fav_server.ip);
    favorite.setValue("port", fav_server.port);
    favorite.setValue("desc", fav_server.desc);

    if (fav_server.socket_type == TCP) {
      favorite.setValue("protocol", "tcp");
    }
    else {
      favorite.setValue("protocol", "ws");
    }
    favorite.endGroup();
  }
  favorite.sync();
}

void Options::removeFavorite(int index)
{
  QVector<server_type> l_favorites = favorites();
  l_favorites.remove(index);
  setFavorites(l_favorites);
}

void Options::addFavorite(server_type server)
{
  int index = favorites().size();
  favorite.beginGroup(QString::number(index));
  favorite.setValue("name", server.name);
  favorite.setValue("address", server.ip);
  favorite.setValue("port", server.port);
  favorite.setValue("desc", server.desc);
  if (server.socket_type == TCP) {
    favorite.setValue("protocol", "tcp");
  }
  else {
    favorite.setValue("protocol", "ws");
  }
  favorite.endGroup();
  favorite.sync();
}

void Options::updateFavorite(server_type server, int index)
{
  favorite.beginGroup(QString::number(index));
  favorite.setValue("name", server.name);
  favorite.setValue("address", server.ip);
  favorite.setValue("port", server.port);
  favorite.setValue("desc", server.desc);
  if (server.socket_type == TCP) {
    favorite.setValue("protocol", "tcp");
  }
  else {
    favorite.setValue("protocol", "ws");
  }
  favorite.endGroup();
  favorite.sync();
}

QString Options::getUIAsset(QString f_asset_name)
{
  QStringList l_paths{":/base/themes/" + Options::getInstance().theme() + "/" +
                      f_asset_name};

  if (subTheme() == "server") {
    if (serverSubTheme().isEmpty()) {
      l_paths.prepend(":/base/themes/" + theme() + "/" + serverSubTheme() +
                      "/" + f_asset_name);
    }
  }
  else {
    l_paths.prepend(":/base/themes/" + theme() + "/" + subTheme() + "/" +
                    f_asset_name);
  }

  for (const QString &l_path : qAsConst(l_paths)) {
    if (QFile::exists(l_path)) {
      return l_path;
    }
  }
  qWarning() << "Unable to locate ui-asset" << f_asset_name << "in theme"
             << theme() << "Defaulting to embeeded asset.";
  return QString(":/resource/ui/" + f_asset_name);
}
