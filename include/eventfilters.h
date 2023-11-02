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

                int expandZoneHeight = 22;

            if (originalMenuBarHeight == -1) {
                originalMenuBarHeight = mainWindow->menuBar()->height();
            }

            if (mainWindowPos.y() <= expandZoneHeight) {
                QRect startRect = QRect(mainWindow->menuBar()->x(), mainWindow->menuBar()->y(), mainWindow->menuBar()->width(), 4);
                QRect endRect = QRect(startRect.x(), startRect.y(), startRect.width(), originalMenuBarHeight);
            
                QPropertyAnimation *animation = new QPropertyAnimation(mainWindow->menuBar(), "geometry");
                animation->setStartValue(startRect);
                animation->setEndValue(endRect);
                animation->setDuration(1000);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            
                QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                    mainWindow->menuBar()->setGeometry(endRect);
                });
            } else {
                QRect startRect = QRect(mainWindow->menuBar()->x(), mainWindow->menuBar()->y(), mainWindow->menuBar()->width(), originalMenuBarHeight);
                QRect endRect = QRect(startRect.x(), startRect.y(), startRect.width(), 4);
            
                QPropertyAnimation *animation = new QPropertyAnimation(mainWindow->menuBar(), "geometry");
                animation->setStartValue(startRect);
                animation->setEndValue(endRect);
                animation->setDuration(1000);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            
                QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                    mainWindow->menuBar()->setGeometry(endRect);
                });
            }
        }
    }
    return false;
  }    
};


#endif // EVENTFILTERS_H
