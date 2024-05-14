#pragma once

#include <QDateTime>
#include <QString>
#include <QtWidgets/QApplication>

class ChatLogPiece
{
  Q_DECLARE_TR_FUNCTIONS(chatlogpiece)

public:
  ChatLogPiece();
  ChatLogPiece(QString p_name, QString p_showname, QString p_message, QString p_action, int color, bool selfname);
  ChatLogPiece(QString p_name, QString p_showname, QString p_message, QString p_action, int color, bool selfname, QDateTime p_datetime);

  QString name;
  QString showname;
  QString message;
  QString action;
  bool selfname = false;
  QDateTime datetime;
  int color = 0;

  QString get_datetime_as_string();
  QString get_full();
};
