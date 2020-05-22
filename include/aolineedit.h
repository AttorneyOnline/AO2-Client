#ifndef AOLINEEDIT_H
#define AOLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class AOLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  AOLineEdit(QWidget *parent);

protected:
  void mouseDoubleClickEvent(QMouseEvent *e);

signals:
  void double_clicked();

private slots:
  void on_enter_pressed();


};

#endif // AOLINEEDIT_H
