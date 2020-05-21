#include "chatlogpiece.h"

chatlogpiece::chatlogpiece()
{
  name = "UNKNOWN";
  showname = "UNKNOWN";
  message = "UNKNOWN";
  p_is_song = false;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, bool p_song)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  p_is_song = p_song;
  datetime = QDateTime::currentDateTime().toUTC();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, bool p_song, QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  p_is_song = p_song;
  datetime = p_datetime.toUTC();
}

QString chatlogpiece::get_name() { return name; }

QString chatlogpiece::get_showname() { return showname; }

QString chatlogpiece::get_message() { return message; }

QDateTime chatlogpiece::get_datetime() { return datetime; }

bool chatlogpiece::is_song() { return p_is_song; }

QString chatlogpiece::get_datetime_as_string() { return datetime.toString(); }

QString chatlogpiece::get_full()
{
  QString full = "[";

  full.append(get_datetime_as_string());
  full.append("] ");
  full.append(get_showname());
  full.append(" (");
  full.append(get_name());
  full.append(")");
  if (p_is_song)
    full.append(" has played a song: ");
  else
    full.append(": ");
  full.append(get_message());

  return full;
}
