#pragma once

#include <QCoreApplication>
#include <QDateTime>
#include <QString>

class ChatLogPiece
{
  Q_DECLARE_TR_FUNCTIONS(ChatLogPiece)

public:
  QString character;
  QString character_name;
  QString message;
  QString action;
  QDateTime timestamp;
  bool local_player = false;
  int color = 0;

  QString toString();
};
