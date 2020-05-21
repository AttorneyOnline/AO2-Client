#ifndef AOLINEEDIT_H
#define AOLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class AOLineEdit : public QLineEdit {
  Q_OBJECT

public:
  AOLineEdit(QWidget *parent);

  void preserve_selection(bool toggle) { p_selection = toggle; }

private:
  bool p_selection = false;

protected:
  void mouseDoubleClickEvent(QMouseEvent *e);
  void focusOutEvent(QFocusEvent *ev);

signals:
  void double_clicked();
};

#endif // AOLINEEDIT_H
