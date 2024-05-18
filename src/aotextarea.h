#pragma once

#include <QDebug>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextCursor>

class AOTextArea : public QTextBrowser
{
  Q_OBJECT

public:
  AOTextArea(QWidget *parent = nullptr);
  AOTextArea(int maximumLogLenth, QWidget *parent = nullptr);

  void addMessage(QString name, QString message, QString nameColor, QString messageColor = QString());

private:
  const QRegularExpression url_parser_regex = QRegularExpression("\\b(https?://\\S+\\.\\S+)\\b");

  void auto_scroll(QTextCursor old_cursor, int scrollbar_value, bool is_scrolled_down);
};
