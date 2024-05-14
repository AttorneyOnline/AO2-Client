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
  AOTextArea(QWidget *p_parent = nullptr);
  AOTextArea(int p_log_length, QWidget *p_parent = nullptr);

  void append_chatmessage(QString p_name, QString p_message, QString p_name_colour, QString p_color = QString());

private:
  const QRegularExpression url_parser_regex = QRegularExpression("\\b(https?://\\S+\\.\\S+)\\b");

  void auto_scroll(QTextCursor old_cursor, int scrollbar_value, bool is_scrolled_down);
};
