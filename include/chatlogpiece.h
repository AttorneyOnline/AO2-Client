#ifndef CHATLOGPIECE_H
#define CHATLOGPIECE_H

#include <QDateTime>
#include <QString>

class chatlogpiece {
public:
  chatlogpiece();
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               bool p_song);
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               bool p_song, QDateTime p_datetime);

  QString get_name();
  QString get_showname();
  QString get_message();
  bool is_song();
  QDateTime get_datetime();
  QString get_datetime_as_string();

  QString get_full();

private:
  QString name;
  QString showname;
  QString message;
  QDateTime datetime;
  bool p_is_song;
};

#endif // CHATLOGPIECE_H
