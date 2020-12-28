#include "options.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QSize>

void Options::migrateCallWords()
{
  // Migrate callwords.ini
  QFile file(QCoreApplication::applicationDirPath() +
             "/base/callwords.ini");

  if (!file.open(QIODevice::ReadOnly))
    return;
  file.rename(QCoreApplication::applicationDirPath() +
              "/base/callwords.old.ini");

  QString list = QString::fromUtf8(file.readAll());
  list.replace(QRegularExpression("\\r"), QStringLiteral());
  auto callWords = list.split('\n', Qt::SkipEmptyParts);
  setCallWords(callWords);
}

/*! Migrate old configuration keys/values to a relevant format. */
void Options::migrate()
{
  migrateCallWords();
  if (config.contains("show_custom_shownames")) {
    setDisableCustomShownames(!config.value("show_custom_shownames", "true").toBool());
    config.remove("show_custom_shownames");
  }
}

/*! The name of the theme to be loaded. */
QString Options::theme() const
{
  return config.value("theme", "default").toString();
}

void Options::setTheme(QString value)
{
  config.setValue("theme", value);
}

/*! The default name to be used in OOC chat. */
QString Options::oocName() const
{
  return config.value("ooc_name").toString();
}

void Options::setOOCName(QString value)
{
  config.setValue("ooc_name", value);
}

/*! The number of characters to print before playing a blip. */
int Options::blipRate() const
{
  return config.value("blip_rate", 1).toInt();
}

void Options::setBlipRate(int value)
{
  config.setValue("blip_rate", value);
}

/*! Play blip sounds on empty characters. */
bool Options::blankBlipsEnabled() const
{
  return config.value("blank_blip", true).toBool();
}

void Options::setBlankBlipsEnabled(bool value)
{
  config.value("blank_blip", value);
}

/*! The default music volume. */
int Options::defaultMusicVolume() const
{
  return config.value("default_music", 50).toInt();
}

void Options::setDefaultMusicVolume(int value)
{
  config.value("default_music", value);
}

/*! The default SFX volume. */
int Options::defaultSfxVolume() const
{
  return config.value("default_sfx", 50).toInt();
}

void Options::setDefaultSfxVolume(int value)
{
  config.setValue("default_sfx", value);
}

/*! The default blip volume. */
int Options::defaultBlipVolume() const
{
  return config.value("default_blip", 50).toInt();
}

void Options::setDefaultBlipVolume(int value)
{
  config.setValue("default_blip", value);
}

/*! Whether or not Discord Rich Presence is enabled on startup. */
bool Options::discordEnabled() const
{
  return config.value("discord", true).toBool();
}

void Options::setDiscordEnabled(bool value)
{
  config.setValue("discord", value);
}

/*! The maximum number of lines to be kept in the IC log. */
int Options::maxLogLines() const
{
  return config.value("log_maximum", 200).toInt();
}

void Options::setMaxLogLines(int value)
{
  config.setValue("log_maximum", value);
}

/*! If enabled, new IC messages will be inserted at the top instead of
 * at the bottom. */
bool Options::legacyScrollEnabled() const
{
  return config.value("log_goes_downwards").toBool();
}

void Options::setLegacyScrollEnabled(bool value)
{
  config.setValue("log_goes_downwards", value);
}

/*!
 * The default audio output device name.
 * Note that this value is passed directly to BASS.
 * TODO: Support QtMultimedia.
 */
QString Options::audioDevice() const
{
  return config.value("default_audio_device", "default").toString();
}

void Options::setAudioDevice(QString value)
{
  config.setValue("default_audio_device", value);
}

/*! Gets a list of call words. The player is notified when any word in
 * this list is mentioned. */
QStringList Options::callWords() const
{
  QStringList list = config.value("callwords", QStringList()).toStringList();
  list.removeAll("");
  return list;
}

void Options::setCallWords(QStringList value)
{
  config.setValue("callwords", value);
}

/*! Adds a server to the favorites list. */
void Options::addToFavoriteServers(const server_type &server)
{
  QFile file(QCoreApplication::applicationDirPath() +
             "/base/serverlist.txt");

  if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    qWarning() << "Could not write server list to file";
    return;
  }

  QTextStream out(&file);
  out << "\r\n" << server.ip << ":" << server.port << ":" << server.name;
  file.close();
}

/*! Gets a list of favorite servers. */
QVector<server_type> Options::favoriteServers() const
{
  QVector<server_type> servers;

  QFile file(QCoreApplication::applicationDirPath() +
             "/base/serverlist.txt");

  if (!file.open(QIODevice::ReadOnly))
  {
    return servers;
  }

  QTextStream in(&file);

  while(!in.atEnd())
  {
    server_type server;
    QStringList entry = in.readLine().split(":");

    if (entry.size() < 3)
        continue;

    server.ip = entry[0];
    server.port = entry[1].toInt();
    server.name = entry[2];

    servers.append(std::move(server));
  }

  return servers;
}

/*! The address (hostname) of the master server. */
QString Options::msAddress() const
{
  return config.value("master", "master.aceattorneyonline.com").toString();
}

void Options::setMSAddress(QString value)
{
  config.setValue("master", value);
}

/*! The port of the master server. */
uint16_t Options::msPort() const
{
  return static_cast<uint16_t>(config.value("master_port", 27016).toInt());
}

void Options::setMSPort(uint16_t value)
{
  config.setValue("master_port", value);
}

QString Options::language() const
{
  return config.value("language", "default").toString();
}

void Options::setLanguage(QString value)
{
  config.setValue("language", value);
}

int Options::blipInterval() const
{
  return config.value("blip_interval", 1).toInt();
}

void Options::setBlipInterval(int value)
{
  config.setValue("blip_interval", value);
}

bool Options::disableLoopingSfx() const
{
  return config.value("disable_looping_sfx", "false").toBool();
}

void Options::setDisableLoopingSfx(bool value)
{
  config.setValue("disable_looping_sfx", value);
}

bool Options::objectionStopsMusic() const
{
  return config.value("objection_stops_music", "false").toBool();
}

void Options::setObjectionStopsMusic(bool value)
{
  config.setValue("objection_stops_music", value);
}

bool Options::logNewlineAfterName() const
{
  return config.value("log_newline_after_name", "false").toBool();
}

void Options::setLogNewlineAfterName(bool value)
{
  config.setValue("log_newline_after_name", value);
}

int Options::logSpacing() const
{
  return config.value("log_spacing", "false").toBool();
}

void Options::setLogSpacing(int value)
{
  config.setValue("log_spacing", value);
}

bool Options::logTimestamps() const
{
  return config.value("log_timestamps", "false").toBool();
}

void Options::setLogTimestamps(bool value)
{
  config.setValue("log_timestamps", value);
}

bool Options::saveLogs() const
{
  return config.value("save_logs", "false").toBool();
}

void Options::setSaveLogs(bool value)
{
  config.setValue("save_logs", value);
}

QSize Options::viewportScale() const
{
  return config.value("viewport_scale", QSize(256, 192)).toSize();
}

void Options::setViewportScale(QSize value)
{
  config.setValue("viewport_scale", value);
}

/*
QString Options::viewportScalingPolicy() const
{
  return config.value("viewport_scaling_policy", "manual").toString();
}

void Options::setViewportScalingPolicy(QString value)
{
  config.setValue("viewport_scaling_policy", value);
}
*/

bool Options::disableScreenshake() const
{
  return config.value("disable_screenshake", "false").toBool();
}

void Options::setDisableScreenshake(bool value)
{
  config.setValue("disable_screenshake", value);
}

bool Options::disableOverlayEffects() const
{
  return config.value("disable_overlay_effects", "false").toBool();
}

void Options::setDisableOverlayEffects(bool value)
{
  config.setValue("disable_overlay_effects", value);
}

bool Options::disableFrameSfx() const
{
  return config.value("disable_frame_sfx", "false").toBool();
}

void Options::setDisableFrameSfx(bool value)
{
  config.setValue("disable_frame_sfx", value);
}

bool Options::disableLogColors() const
{
  return config.value("disable_log_colors", "false").toBool();
}

void Options::setDisableLogColors(bool value)
{
  config.setValue("disable_log_colors", value);
}

bool Options::disableCustomChatboxes() const
{
  return config.value("disable_custom_chatboxes", "false").toBool();
}

void Options::setDisableCustomChatboxes(bool value)
{
  config.setValue("disable_custom_chatboxes", value);
}

bool Options::disableCustomShownames() const
{
  return config.value("disable_custom_shownames", "false").toBool();
}

void Options::setDisableCustomShownames(bool value)
{
  config.setValue("disable_custom_shownames", value);
}

bool Options::stickyPreanim() const
{
  return config.value("sticky_preanim", "false").toBool();
}

void Options::setStickyPreanim(bool value)
{
  config.setValue("sticky_preanim", value);
}

bool Options::stickySfx() const
{
  return config.value("sticky_sfx", "false").toBool();
}

void Options::setStickySfx(bool value)
{
  config.setValue("sticky_sfx", value);
}

bool Options::stickyEffects() const
{
  return config.value("sticky_effects", "false").toBool();
}

void Options::setStickyEffects(bool value)
{
  config.setValue("sticky_effects", value);
}
