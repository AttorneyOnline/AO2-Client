#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include "options.h"

#include <QEvent>
#include <QLineEdit>
#include <QMenuBar>
#include <QTimer>
#include <QDebug>
#include <QMainWindow>

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
    int originalMenuBarHeight = -1;
private:
    QPropertyAnimation* animation;
    bool entered_zone = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(obj);

        if (mainWindow != nullptr && !Options::getInstance().menuBarLocked()) {
            if (event->type() == QEvent::HoverMove) {
                QPoint globalPos = QCursor::pos();
                QPoint mainWindowPos = mainWindow->mapFromGlobal(globalPos);

                int expandZoneHeight = 22;

                if (originalMenuBarHeight == -1) {
                    originalMenuBarHeight = mainWindow->menuBar()->height();
                }

                if (mainWindowPos.y() <= expandZoneHeight) {
                    if (!entered_zone)
                        mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                        entered_zone = true;
                } else {
                    if (entered_zone)
                        QTimer::singleShot(1000, mainWindow, [this, mainWindow]() {
                            mainWindow->menuBar()->setFixedHeight(3);
                            entered_zone = false;
                    });
                }
            }
        }
        return false;
    }
};


#endif // EVENTFILTERS_H
