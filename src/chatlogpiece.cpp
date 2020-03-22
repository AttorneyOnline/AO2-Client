#include "chatlogpiece.h"

chatlogpiece::chatlogpiece()
{
  name = "UNKNOWN";
  showname = "UNKNOWN";
  message = "UNKNOWN";
  color = 0;
  is_song = false;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song, int p_color)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  is_song = p_song;
  color = p_color;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song,int p_color, QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  is_song = p_song;
  color = p_color;
  datetime = p_datetime.toUTC();
}

QString chatlogpiece::get_name()
{
  return name;
}

QString chatlogpiece::get_showname()
{
  return showname;
}

QString chatlogpiece::get_message()
{
  return message;
}

QDateTime chatlogpiece::get_datetime()
{
  return datetime;
}

bool chatlogpiece::get_is_song()
{
  return is_song;
}

QString chatlogpiece::get_datetime_as_string()
{
  return datetime.toString();
}
int chatlogpiece::get_chat_color()
{
    return color;
}

QString chatlogpiece::get_full()
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
