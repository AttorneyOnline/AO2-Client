#include "chatlogpiece.h"

chatlogpiece::chatlogpiece()
{
  name = tr("UNKNOWN");
  showname = tr("UNKNOWN");
  message = tr("UNKNOWN");
  color = 0;
  action = "";
  datetime = QDateTime::currentDateTimeUtc();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, QString p_action, int p_color)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  datetime = QDateTime::currentDateTimeUtc();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, QString p_action, int p_color,
                           QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  datetime = p_datetime.toUTC();
}

QString chatlogpiece::get_name() { return name; }

QString chatlogpiece::get_showname() { return showname; }

QString chatlogpiece::get_message() { return message; }

QDateTime chatlogpiece::get_datetime() { return datetime; }

QString chatlogpiece::get_action() { return action; }

QString chatlogpiece::get_datetime_as_string() { return datetime.toString(); }

int chatlogpiece::get_chat_color() { return color; }

QString chatlogpiece::get_full()
{
  QString full = "[";

  full.append(get_datetime_as_string());
  full.append("] ");
  full.append(get_showname());
  if (get_showname() != get_name())
  {
    full.append(" (");
    full.append(get_name());
    full.append(")");
  }
  if (!get_action().isEmpty())
    full.append(" " + get_action());
  full.append(": ");
  full.append(get_message());

  return full;
}
