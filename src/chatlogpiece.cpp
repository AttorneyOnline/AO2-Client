#include "chatlogpiece.h"

chatlogpiece::chatlogpiece()
{
  name = "UNKNOWN";
  showname = "UNKNOWN";
  message = "UNKNOWN";
  is_song = false;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  is_song = p_song;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song, QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  is_song = p_song;
  datetime = p_datetime.toUTC();
}

QString chatlogpiece::get_name() const
{
  return name;
}

QString chatlogpiece::get_showname() const
{
  return showname;
}

QString chatlogpiece::get_message() const
{
  return message;
}

QDateTime chatlogpiece::get_datetime() const
{
  return datetime;
}

bool chatlogpiece::get_is_song()
{
  return is_song;
}

QString chatlogpiece::get_datetime_as_string() const
{
  return datetime.toString();
}


QString chatlogpiece::get_full() const
{
  QString full = "[";

  full.append(get_datetime_as_string());
  full.append(" UTC] ");
  full.append(get_showname());
  full.append(" (");
  full.append(get_name());
  full.append(")");
  if (is_song)
    full.append(" has played a song: ");
  full.append(get_message());

  return full;
}
