#ifndef AOTEXTAREA_H
#define AOTEXTAREA_H

#include <QDebug>
#include <QRegExp>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextCursor>

class AOTextArea : public QTextBrowser {
public:
  AOTextArea(QWidget *p_parent = nullptr);

  void append_chatmessage(QString p_name, QString p_message, QString p_colour,
                          bool song);
  void append_error(QString p_message);

private:
  const QRegExp omnis_dank_url_regex = QRegExp("\\b(https?://\\S+\\.\\S+)\\b");

  void auto_scroll(QTextCursor old_cursor, int scrollbar_value,
                   bool is_scrolled_down);
};

#endif // AOTEXTAREA_H
