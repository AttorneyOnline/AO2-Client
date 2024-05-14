#include "chatlogpiece.h"

ChatLogPiece::ChatLogPiece()
{
  name = tr("UNKNOWN");
  showname = tr("UNKNOWN");
  message = tr("UNKNOWN");
  color = 0;
  datetime = QDateTime::currentDateTimeUtc();
}

ChatLogPiece::ChatLogPiece(QString p_name, QString p_showname, QString p_message, QString p_action, int p_color, bool p_selfname)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  selfname = p_selfname;
  datetime = QDateTime::currentDateTimeUtc();
}

ChatLogPiece::ChatLogPiece(QString p_name, QString p_showname, QString p_message, QString p_action, int p_color, bool p_selfname, QDateTime p_datetime)
{
  name = p_name;
  showname = p_showname;
  message = p_message;
  action = p_action;
  color = p_color;
  selfname = p_selfname;
  datetime = p_datetime.toUTC();
}

QString ChatLogPiece::get_datetime_as_string()
{
  return datetime.toString();
}

QString ChatLogPiece::get_full()
{
  QString full = "[";

  full.append(get_datetime_as_string());
  full.append("] ");
  full.append(showname);
  if (showname != name)
  {
    full.append(" (");
    full.append(name);
    full.append(")");
  }
  if (!action.isEmpty())
  {
    full.append(" " + action);
  }
  full.append(": ");
  full.append(message);

  return full;
}
