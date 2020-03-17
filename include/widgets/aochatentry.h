#ifndef AOCHATENTRY_H
#define AOCHATENTRY_H

#include <QObject>
#include <QTextEdit>

class AOChatEntry : public QTextEdit
{
  Q_OBJECT
public:
  AOChatEntry(QWidget *parent) : QTextEdit(parent) {}

protected:
  void keyPressEvent(QKeyEvent *event) override;

signals:
  void enterPressed();
};

#endif // AOCHATENTRY_H
