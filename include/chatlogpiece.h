#ifndef CHATLOGPIECE_H
#define CHATLOGPIECE_H

#include <QtWidgets/QApplication>
#include <QDateTime>
#include <QString>

class chatlogpiece {
    Q_DECLARE_TR_FUNCTIONS(chatlogpiece)
public:
  chatlogpiece();
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               QString p_action,int color, bool selfname);
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               QString p_action, int color, bool selfname, QDateTime p_datetime);

  QString get_name() { return name; };
  QString get_showname() { return showname; };
  QString get_message() { return message; };
  QString get_action() { return action; };
  bool get_selfname() const { return selfname; };
  QDateTime get_datetime() { return datetime; };
  QString get_datetime_as_string() { return datetime.toString(); };
  int get_chat_color() const { return color; };
  QString get_full();

private:
  QString name;
  QString showname;
  QString message;
  QString action;
  bool selfname;
  QDateTime datetime;
  int color;
};

#endif // CHATLOGPIECE_H
