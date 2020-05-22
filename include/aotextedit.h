#ifndef AOTEXTEDIT_H
#define AOTEXTEDIT_H

#include <QPlainTextEdit>

class AOTextEdit : public QPlainTextEdit
{
  Q_OBJECT
public:
  AOTextEdit(QWidget *parent);

protected:
  void mouseDoubleClickEvent(QMouseEvent *e);

signals:
  void double_clicked();

private slots:
  void on_enter_pressed();

};

#endif // AOTEXTEDIT_H
