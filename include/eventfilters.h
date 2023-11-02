#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include <QEvent>
#include <QLineEdit>
#include <QMenuBar>
#include <QPropertyAnimation>

class AOLineEditFilter : public QObject
{
    Q_OBJECT
public:
    bool preserve_selection = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
       if (event->type() == QEvent::FocusOut && lineEdit != nullptr && preserve_selection) { // lost focus
            int start = lineEdit->selectionStart();
          #if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            int len = lineEdit->selectionLength();
          #else
            int len = lineEdit->selectedText().length();
          #endif
            if (start != -1 && len != -1) {
              lineEdit->setSelection(start, len);\
              return true;
            }
        }
        return false;
    }
signals:
    void double_clicked();
};

class QMenuBarFilter : public QObject
{
    Q_OBJECT

public:
    bool collapseMenuBar = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QMenuBar *menuBar = qobject_cast<QMenuBar *>(obj);
        if (menuBar != nullptr && collapseMenuBar) {
            if (event->type() == QEvent::Enter) {
                qDebug() << "Mouse entró en el QMenuBar";
                // We expand the QMenuBar
                QPropertyAnimation *animation = new QPropertyAnimation(menuBar, "geometry");
                animation->setDuration(500);
                QRect startRect = menuBar->geometry();
                QRect endRect = QRect(0, 0, menuBar->width(), menuBar->height());
                animation->setStartValue(startRect);
                animation->setEndValue(endRect);
                animation->start();
            } else if (event->type() == QEvent::Leave) {
                qDebug() << "Mouse salió del QMenuBar";
                // If we leave, retract the QMenuBar
                QPropertyAnimation *animation = new QPropertyAnimation(menuBar, "geometry");
                animation->setDuration(500);
                QRect startRect = menuBar->geometry();
                QRect endRect = QRect(0, 0, menuBar->width(), 2);
                animation->setStartValue(startRect);
                animation->setEndValue(endRect);
                animation->start();
            }
        }
        return false;
    }
};

#endif // EVENTFILTERS_H
