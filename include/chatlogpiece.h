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
               QString p_action,int color);
  chatlogpiece(QString p_name, QString p_showname, QString p_message,
               QString p_action, int color, QDateTime p_datetime);

  QString get_name();
  QString get_showname();
  QString get_message();
  QString get_action();
  QDateTime get_datetime();
  QString get_datetime_as_string();
  int get_chat_color();
  QString get_full();

private:
  QString name;
  QString showname;
  QString message;
  QString action;
  QDateTime datetime;
  int color;
};

#endif // CHATLOGPIECE_H
