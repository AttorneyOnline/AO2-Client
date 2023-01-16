#include "chatlogpiece.h"

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, QString p_action, int p_color, bool p_selfname)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  selfname = p_selfname;
  datetime = QDateTime::currentDateTimeUtc();
}

chatlogpiece::chatlogpiece(QString p_name, QString p_showname,
                           QString p_message, QString p_action, int p_color, bool p_selfname,
                           QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  selfname = p_selfname;
  datetime = p_datetime.toUTC();
}

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
