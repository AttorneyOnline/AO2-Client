#ifndef CHATLOGPIECE_H
#define CHATLOGPIECE_H

#include <QString>
#include <QDateTime>

class chatlogpiece
{
public:
  chatlogpiece();
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               bool p_song, int color);
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               bool p_song, int color, QDateTime p_datetime);
  QString get_name();
  QString get_showname();
  QString get_message();
  bool get_is_song();
  QDateTime get_datetime();
  QString get_datetime_as_string();
  int get_chat_color();

  QString get_full();

private:
  QString name;
  QString showname;
  QString message;
  QDateTime datetime;
  bool is_song;
  int color;
};

#endif // CHATLOGPIECE_H
