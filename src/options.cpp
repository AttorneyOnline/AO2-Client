#include "options.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>

/*! The name of the theme to be loaded. */
QString Options::theme() const
{
  return config.value("theme", "default").toString();
}

/*! The default name to be used in OOC chat. */
QString Options::oocName() const
{
  return config.value("ooc_name").toString();
}

/*! The number of characters to print before playing a blip. */
int Options::blipRate() const
{
  return config.value("blip_rate", 1).toInt();
}

/*! Play blip sounds on empty characters. */
bool Options::blankBlipsEnabled() const
{
  return config.value("blank_blip", true).toBool();
}

/*! The default music volume. */
int Options::defaultMusicVolume() const
{
  return config.value("default_music", 50).toInt();
}

/*! The default SFX volume. */
int Options::defaultSfxVolume() const
{
  return config.value("default_sfx", 50).toInt();
}

/*! The default blip volume. */
int Options::defaultBlipVolume() const
{
  return config.value("default_blip", 50).toInt();
}

/*! Whether or not Discord Rich Presence is enabled on startup. */
bool Options::discordEnabled() const
{
  return config.value("discord", true).toBool();
}

/*! The maximum number of lines to be kept in the IC log. */
int Options::maxLogLines() const
{
  return config.value("log_maximum", 200).toInt();
}

/*! If enabled, new IC messages will be inserted at the top instead of
 * at the bottom. */
bool Options::legacyScrollEnabled() const
{
  return config.value("log_goes_downwards").toBool();
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

/*! Whether or not custom shownames are enabled by default. */
bool Options::shownamesEnabled() const
{
  return config.value("show_custom_shownames", true).toBool();
}

/*! Gets a list of call words. The player is notified when any word in
 * this list is mentioned. */
QStringList Options::callWords() const
{
  QFile file(QCoreApplication::applicationDirPath() +
             "/base/callwords.ini");

  if (!file.open(QIODevice::ReadOnly))
    return QStringList();

  QString list = QString::fromUtf8(file.readAll());
  list.replace(QRegularExpression("\\r"), QStringLiteral());
  return list.split('\n', Qt::SkipEmptyParts);
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

/*! The port of the master server. */
uint16_t Options::msPort() const
{
  return static_cast<uint16_t>(config.value("master_port", 27016).toInt());
}

bool Options::casingRoleEnabled(const QString &role) const
{
  return config.value(QStringLiteral("casing_%1_enabled").arg(role), true).toBool();
}
