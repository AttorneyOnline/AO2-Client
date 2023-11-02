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

            QRect menuBarGeometry = mainWindow->menuBar()->geometry();

            if (mainWindowPos.y() <= expandZoneHeight) {
                QPropertyAnimation *animation = new QPropertyAnimation(mainWindow->menuBar(), "geometry");
                animation->setStartValue(menuBarGeometry);
                QRect endGeometry = menuBarGeometry;
                endGeometry.setHeight(originalMenuBarHeight);
                animation->setEndValue(endGeometry);
                animation->setDuration(1000);
                animation->start(QAbstractAnimation::DeleteWhenStopped);

                QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                    mainWindow->menuBar()->setGeometry(endGeometry);
                });
            } else {
                QPropertyAnimation *animation = new QPropertyAnimation(mainWindow->menuBar(), "geometry");
                animation->setStartValue(menuBarGeometry);
                QRect endGeometry = menuBarGeometry;
                endGeometry.setHeight(4);
                animation->setEndValue(endGeometry);
                animation->setDuration(1000);
                animation->start(QAbstractAnimation::DeleteWhenStopped);

                QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
                    mainWindow->menuBar()->setGeometry(endGeometry);
                });
            }
        }
    }
    return false;
  }    
};


#endif // EVENTFILTERS_H
