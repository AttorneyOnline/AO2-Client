#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include <QEvent>
#include <QLineEdit>
#include <QMenuBar>
#include <QPropertyAnimation>
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

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(obj);

        if (mainWindow != nullptr) {
            if (event->type() == QEvent::HoverMove) {
                QPoint globalPos = QCursor::pos();
                QPoint mainWindowPos = mainWindow->mapFromGlobal(globalPos);

                int expandZoneHeight = 20;

                if (originalMenuBarHeight == -1) {
                    originalMenuBarHeight = mainWindow->menuBar()->height();
                }

                if (mainWindowPos.y() <= expandZoneHeight) {
                    qDebug() << mainWindowPos.y();
                    mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                } else {
                    qDebug() << "else: " << mainWindowPos.y();
                    mainWindow->menuBar()->setFixedHeight(4);
                }
            }
        }
        return false;
    }
};


#endif // EVENTFILTERS_H
