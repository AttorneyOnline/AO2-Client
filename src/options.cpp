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

QString Options::theme()
{
  return config.value("theme", "default").toString();
}

int Options::blipRate()
{
  return config.value("blip_rate", 2).toInt();
}

QString Options::oocName()
{
  return config.value("ooc_name").toString();
}

int Options::musicVolume()
{
  return config.value("default_music", 50).toInt();
}

int Options::sfxVolume()
{
  return config.value("default_sfx", 50).toInt();
}

int Options::blipVolume()
{
  return config.value("default_blip", 50).toInt();
}

int Options::defaultsuppressaudio()
{
  return config.value("suppress_audio", 50).toInt();
}

int Options::maxLogSize()
{
  return config.value("log_maximum", 200).toInt();
}

int Options::textStayTime()
{
  return config.value("stay_time", 200).toInt();
}

int Options::textCrawlSpeed()
{
  return config.value("text_crawl", 40).toInt();
}

int Options::chatRateLimit()
{
  return config.value("chat_ratelimit", 300).toInt();
}

bool Options::logDirectionDownwards()
{
  return config.value("log_goes_downwards", true).toBool();
}

bool Options::logNewline()
{
  return config.value("log_newline", false).toBool();
}

int Options::logMargin()
{
  return config.value("log_margin", 0).toInt();
}

bool Options::logTimestampEnabled()
{
  return config.value("log_timestamp", false).toBool();
}

QString Options::logTimestampFormat()
{
  return config.value("log_timestamp_format", "h:mm:ss AP").toString();
}

bool Options::logIcActions()
{
  return config.value("log_ic_actions", true).toBool();
}

bool Options::customShownameEnabled()
{
  return config.value("show_custom_shownames", true).toBool();
}

QString Options::username()
{
 // Reimplement me!
 return "";
}

QString Options::shownameOnJoin()
{
  return config.value("default_showname", "").toString();
}

QString Options::audioOutputDevice()
{
  return config.value("default_audio_device", "default").toString();
}

bool Options::blankBlip()
{
  return config.value("blank_blip", false).toBool();
}

bool Options::loopingSfx()
{
  return config.value("looping_sfx", true).toBool();
}

bool Options::objectionStopMusic()
{
  return config.value("objection_stop_music", false).toBool();
}

bool Options::streamingEnabled()
{
  return config.value("streaming_disabled", false).toBool();
}

bool Options::objectionSkipQueueEnabled()
{
  return config.value("instant_objection", true).toBool();
}

bool Options::desynchronisedLogsEnabled()
{
  return config.value("desync_logs", false).toBool();

}

bool Options::discordEnabled()
{
  return config.value("discord", true).toBool();

}

bool Options::shakeEnabled()
{
  return config.value("shake", true).toBool();

}

bool Options::effectsEnabled()
{
  return config.value("effects", true).toBool();

}

bool Options::networkedFrameSfxEnabled()
{
  return config.value("framenetwork", true).toBool();

}

bool Options::colorLogEnabled()
{
  return config.value("colorlog", true).toBool();
}

bool Options::clearSoundsDropdownOnPlayEnabled()
{
  return config.value("stickysounds", true).toBool();
}

bool Options::clearEffectsDropdownOnPlayEnabled()
{
  return config.value("stickyeffects", true).toBool();
}

bool Options::clearPreOnPlayEnabled()
{
  return config.value("stickypres", true).toBool();

}

bool Options::customChatboxEnabled()
{
  return config.value("customchat", true).toBool();
}

bool Options::characterStickerEnabled()
{
  return config.value("sticker", true).toBool();
}

bool Options::continuousPlaybackEnabled()
{
  return config.value("continuous_playback", true).toBool();
}

bool Options::stopMusicOnCategoryEnabled()
{
  return config.value("category_stop", true).toBool();
}

bool Options::casingAlertEnabled()
{
  return config.value("casing_enabled", false).toBool();
}

bool Options::casingDefenceEnabled()
{
  return config.value("casing_defence_enabled", false).toBool();
}

bool Options::casingProsecutionEnabled()
{
  return config.value("casing_prosecution_enabled", false).toBool();
}

bool Options::casingJudgeEnabled()
{
  return config.value("casing_judge_enabled", false).toBool();
}

bool Options::casingJurorEnabled()
{
  return config.value("casing_juror_enabled", false).toBool();
}

bool Options::casingStenoEnabled()
{
  return config.value("casing_steno_enabled", false).toBool();
}

bool Options::casingCmEnabled()
{
  return config.value("casing_cm_enabled", false).toBool();
}

QString Options::casingCanHostCases()
{
  return config.value("casing_can_host_cases", "Turnabout Check Your Settings").toString();
}

bool Options::logToTextFileEnabled()
{
  return config.value("automatic_logging_enabled", true).toBool();
}

bool Options::logToDemoFileEnabled()
{
  return config.value("demo_logging_enabled", true).toBool();
}

QString Options::subTheme()
{
  return config.value("subtheme", "server").toString();
}

bool Options::animatedThemeEnabled()
{
  return config.value("animated_theme", true).toBool();
}

QString Options::DefaultScalingMode()
{
  return config.value("default_scaling", "fast").toString();
}

QStringList Options::mountpaths()
{
  return config.value("mount_paths").value<QStringList>();
}

bool Options::PlayercountOptout()
{
  return config.value("player_count_optout", false).toBool();
}

bool Options::SfxonIdle()
{
  return config.value("sfx_on_idle", false).toBool();
}

bool Options::evidenceDoubleClickEdit()
{
  return config.value("evidence_double_click", true).toBool();
}

