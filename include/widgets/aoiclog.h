#ifndef AOICLOG_H
#define AOICLOG_H

#include "aoapplication.h"
#include "chatlogpiece.h"

#include <QTextEdit>
#include <QWidget>

class AOICLog : public QWidget
{
  Q_OBJECT
public:
  explicit AOICLog(QWidget *parent, AOApplication *p_ao_app);

  //adds text to the IC chatlog. p_name first as bold then p_text then a newlin
  //this function keeps the chatlog scrolled to the top unless there's text selected
  // or the user isn't already scrolled to the top
  void append_ic_text(QString p_default_showname, QString p_showname, QString p_name, bool is_songchange = false);

  void reload();
  void export_to_file(QFile &file);
private:
  AOApplication *ao_app;
  Options options;

  QTextEdit *ui_ic_chatlog;
  QVector<chatlogpiece> ic_chatlog_history;

  //This function filters out the common CC inline text trickery, for appending to
  //the IC chatlog.
  QString filter_ic_text(QString p_text);
signals:

public slots:
};

#endif // AOICLOG_H
