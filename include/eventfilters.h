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
private:
    QPropertyAnimation* animation;
    bool entered_zone = false;

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
    
                if (mainWindowPos.y() <= expandZoneHeight && !entered_zone) {
                    qDebug() << "Moved mouse inside zone, animation not running: " << mainWindowPos.y();
                    entered_zone = true;
                    QPoint p = mainWindow->menuBar()->pos();
                    ///QRect startRect = QRect(mainWindow->menuBar()->x(), mainWindow->menuBar()->y(), mainWindow->menuBar()->width(), 4);
                    //QRect endRect = QRect(startRect.x(), startRect.y(), startRect.width(), originalMenuBarHeight);
    
                    animation = new QPropertyAnimation(mainWindow->menuBar(), "pos");
                    animation->setStartValue(p);
                    p += QPoint(0, 30);
                    animation->setEndValue(p);
                    animation->setDuration(500);
                    animation->start(QAbstractAnimation::DeleteWhenStopped);
                    
                    QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                        // mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                        qDebug() << "Out animation done!"
                    });
                } else if (mainWindowPos.y() > expandZoneHeight && entered_zone) {
                    entered_zone = false;
                    qDebug() << "Moved mouse outside zone, animation not running: " << mainWindowPos.y();
                    QPoint p = mainWindow->menuBar()->pos();
                    //QRect startRect = QRect(mainWindow->menuBar()->x(), mainWindow->menuBar()->y(), mainWindow->menuBar()->width(), originalMenuBarHeight);
                    //QRect endRect = QRect(startRect.x(), startRect.y(), startRect.width(), 4);
    
                    animation = new QPropertyAnimation(mainWindow->menuBar(), "pos");
                    animation->setStartValue(p);
                    p += QPoint(0, -30);
                    animation->setEndValue(p);
                    animation->setDuration(500);
                    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
                    QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                        // mainWindow->menuBar()->setFixedHeight(4);
                        // animationRunning = false;
                        qDebug() << "Retract animation done!"
                    });
                }
            }
        }
    return false;
  }    
};


#endif // EVENTFILTERS_H
