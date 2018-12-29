#ifndef AOSERVERCHAT_H
#define AOSERVERCHAT_H

#include "aoapplication.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>

struct CursorState {
  QTextCursor cursor;
  int scrollbar_value;
  bool is_scrolled_down;
};

class AOServerChat : public QWidget
{
  Q_OBJECT
public:
  explicit AOServerChat(QWidget *parent, AOApplication *p_ao_app);

  QString name() { return ui_name->text(); }
  void set_name(QString name) { ui_name->setText(name); }
  QString chat_log() { return ui_chatlog->toPlainText(); }
private:
  AOApplication *ao_app;

  QLineEdit *ui_message;
  QLineEdit *ui_name;

  QTextBrowser *ui_chatlog;

  const QRegExp omnis_dank_url_regex = QRegExp("\\b(https?://\\S+\\.\\S+)\\b");

  void send_message();

  static CursorState get_scroll_state(QTextEdit *textArea);
  void auto_scroll(CursorState &old_state);
signals:
  void messageSent(QString name, QString message);

public slots:
  void append_chat_message(QString name, QString message, QString color);
  void append_error(QString message);
  void append_text(QString message);
};

#endif // AOSERVERCHAT_H
