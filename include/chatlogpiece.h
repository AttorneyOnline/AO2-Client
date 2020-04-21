#ifndef CHATLOGPIECE_H
#define CHATLOGPIECE_H

#include <QString>
#include <QDateTime>

class chatlogpiece
{
public:
  chatlogpiece();
  chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song);
  chatlogpiece(QString p_name, QString p_showname, QString p_message, bool p_song, QDateTime p_datetime);

  QString get_name() const;
  QString get_showname() const;
  QString get_message() const;
  bool get_is_song();
  QDateTime get_datetime() const;
  QString get_datetime_as_string() const;

  QString get_full() const;

private:
  QString name;
  QString showname;
  QString message;
  QDateTime datetime;
  bool is_song;
};

#endif // CHATLOGPIECE_H
