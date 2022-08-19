#include "options.h"

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QSize>

Options::Options() : config(
                         QCoreApplication::applicationDirPath() + "/base/config.ini",
                         QSettings::IniFormat)
{
  Options::options = this;
  migrate();
}

/*! Migrate old configuration keys/values to a relevant format. */
void Options::migrate()
{
  if (config.contains("show_custom_shownames")) {
    config.remove("show_custom_shownames");
  }
}

QString Options::theme() const
{
  return config.value("theme", "default").toString();
}

void Options::setTheme(QString value)
{
  config.setValue("theme", value);
}

int Options::blipRate() const
{
  return config.value("blip_rate", 2).toInt();
}

void Options::setBlipRate(int value)
{
  config.setValue("blip_rate", value);
}

QString Options::oocName() const
{
  return config.value("ooc_name").toString();
}

void Options::setOocName(QString value)
{
  config.setValue("ooc_name", value);
}

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

void Options::setSfxVolume(int value)
{
  config.setValue("default_sfx", value);
}

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

void Options::setLognewline(bool value)
{
  config.setValue("log_newline", value);
}

int Options::logMargin() const
{
    return config.value("log_margin", 0).toInt();
}

void Options::setLogMargin(int value)
{
  config.setValue("log_margin", value);
}

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
 // Reimplement me!
    return "";
}

void Options::setUsername(QString value)
{
  config.setValue("ooc_name", value);
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

void Options::setBlankBlip(bool value)
{
  config.setValue("blank_blip", value);
}

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
    return config.value("streaming_disabled", false).toBool();
}

void Options::setStreamingEnabled(bool value)
{
  config.setValue("streaming_disabled", value);
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

void Options::setShakeEnabled(bool value)
{
  config.setValue("shake", value);
}

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

bool Options::clearPreOnPlayEnabled()  const
{
  return config.value("stickypres", true).toBool();

}

void Options::setClearPreOnPlayEnabled(bool value)
{
  config.setValue("stickypres", value);
}

bool Options::customChatboxEnabled()  const
{
  return config.value("customchat", true).toBool();
}

void Options::setCustomChatboxEnabled(bool value)
{
  config.setValue("customchat", value);
}

bool Options::characterStickerEnabled()  const
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

bool Options::casingAlertEnabled() const
{
  return config.value("casing_enabled", false).toBool();
}

void Options::setCasingAlertEnabled(bool value)
{
  config.setValue("casing_enabled", value);
}

bool Options::casingDefenceEnabled() const
{
  return config.value("casing_defence_enabled", false).toBool();
}

void Options::setcasingDefenceEnabled(bool value)
{
  config.setValue("casing_defence_enabled", value);
}

bool Options::casingProsecutionEnabled() const
{
  return config.value("casing_prosecution_enabled", false).toBool();
}

void Options::setCasingProseuctionEnabled(bool value)
{
  config.setValue("casing_prosecution_enabled", value);
}

bool Options::casingJudgeEnabled() const
{
  return config.value("casing_judge_enabled", false).toBool();
}

void Options::setCasingJudgeEnabled(bool value)
{
  config.setValue("casing_judge_enabled", value);
}

bool Options::casingJurorEnabled() const
{
  return config.value("casing_juror_enabled", false).toBool();
}

void Options::setCasingJurorEnabled(bool value)
{
  config.setValue("casing_juror_enabled", value);
}

bool Options::casingStenoEnabled() const
{
  return config.value("casing_steno_enabled", false).toBool();
}

void Options::setCasingStenoEnabled(bool value)
{
  config.setValue("casing_steno_enabled", value);
}

bool Options::casingCmEnabled() const
{
  return config.value("casing_cm_enabled", false).toBool();
}

void Options::setCasingCmEnabled(bool value)
{
  config.setValue("casing_cm_enabled", value);
}

QString Options::casingCanHostCases() const
{
  return config.value("casing_can_host_cases", "Turnabout Check Your Settings").toString();
}

void Options::setCasingCanHostCases(QString value)
{
  config.setValue("casing_can_host_cases", value);
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
  return config.value("subtheme", "server").toString();
}

void Options::setSubTheme(QString value)
{
  config.setValue("subtheme", value);
}

bool Options::animatedThemeEnabled() const
{
  return config.value("animated_theme", true).toBool();
}

void Options::setAnimatedThemeEnabled(bool value)
{
  config.setValue("animated_theme", value);
}

QString Options::DefaultScalingMode() const
{
  return config.value("default_scaling", "fast").toString();
}

void Options::setDefaultScalingMode(QString value)
{
  config.setValue("default_scaling", value);
}

QStringList Options::mountpaths() const
{
  return config.value("mount_paths").value<QStringList>();
}

void Options::setMountpaths(QStringList value)
{
  config.setValue("mount_paths", value);
}

bool Options::PlayercountOptout() const
{
  return config.value("player_count_optout", false).toBool();
}

void Options::SetPlayerCountOptout(bool value)
{
  config.setValue("player_count_optout", value);
}

bool Options::SfxonIdle() const
{
  return config.value("sfx_on_idle", false).toBool();
}

void Options::setSfxOnIdle(bool value)
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

