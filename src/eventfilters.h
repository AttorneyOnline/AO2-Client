#pragma once

#include <QEvent>
#include <QLineEdit>

class AOLineEditFilter : public QObject
{
  Q_OBJECT

public:
  bool preserve_selection = false;

Q_SIGNALS:
  void double_clicked();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
};
