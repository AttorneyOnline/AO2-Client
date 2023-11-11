#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include "aoapplication.h"
#include "options.h"
#include "courtroom.h"

#include <QEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenuBar>
#include <QTimer>
#include <QDebug>
#include <QMainWindow>

class AOLineEditFilter : public QObject
{
    Q_OBJECT
public:
    //AOLineEditFilter(QWidget *p_parent, AOApplication *p_ao_app);
    bool preserve_selection = false;

private:
    AOApplication *ao_app;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QTextEdit *textEdit = qobject_cast<QTextEdit *>(obj);
        AOApplication *aoApp = qobject_cast<AOApplication *>(qApp);
    
        if (textEdit != nullptr && aoApp != nullptr) {
            // Key press detection
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
                if (keyEvent->key() == Qt::Key_Return) {
                    qDebug("Enter Key Pressed..."); // Debug it for now
                    if (aoApp->w_courtroom != nullptr) {
                        aoApp->w_courtroom->on_chat_return_pressed();
                    }
                    return true;
                 }
              }

            // Focus-out event handling
            if (event->type() == QEvent::FocusOut && preserve_selection) {
                QTextCursor cursor = textEdit->textCursor();
                int start = cursor.selectionStart();
                int len = cursor.selectionEnd() - start;
                if (start != -1 && len != -1) {
                    cursor.setPosition(start);
                    cursor.setPosition(start + len, QTextCursor::KeepAnchor);
                    textEdit->setTextCursor(cursor);
                    return true;
                }
            }
        }
        return false;
    }
};

class QMenuBarFilter : public QObject
{
    Q_OBJECT

public:
    bool collapseMenuBar = false;
    int originalMenuBarHeight = -1;
private:
    // QPropertyAnimation* animation;
    bool entered_zone = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(obj);

        if (mainWindow != nullptr) {
            if (event->type() == QEvent::HoverMove) {
                if (originalMenuBarHeight == -1) {
                    originalMenuBarHeight = mainWindow->menuBar()->height();
                }
                if (Options::getInstance().menuBarLocked()) {
                    mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                    return false;
                }

                QPoint globalPos = QCursor::pos();
                QPoint mainWindowPos = mainWindow->mapFromGlobal(globalPos);

                int expandZoneHeight = 22;

                if (mainWindowPos.y() <= expandZoneHeight) {
                    if (!entered_zone)
                        mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                        entered_zone = true;
                } else {
                    if (entered_zone)
                        mainWindow->menuBar()->setFixedHeight(0);
                        entered_zone = false;
                }
            }
        }
        return false;
    }
};


#endif // EVENTFILTERS_H
