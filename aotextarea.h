#ifndef AOTEXTAREA_H
#define AOTEXTAREA_H

#include <QTextBrowser>

class AOTextArea : public QTextBrowser
{
public:
  AOTextArea(QWidget *p_parent = nullptr);

  void append_chatmessage(QString p_name, QString p_message);
};

#endif // AOTEXTAREA_H
