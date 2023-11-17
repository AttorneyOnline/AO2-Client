#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include "options.h"

#include <QEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenuBar>
// #include <QTimer>
#include <QDebug>
#include <QMainWindow>
#include <QAbstractItemView>
#include <QDesktopWidget>

class AOLineEditFilter : public QObject
{
    Q_OBJECT
public:
    bool preserve_selection = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
        QAbstractItemView *view = qobject_cast<QAbstractItemView*>(obj);
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
        if (lineEdit && view && event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            if (keyEvent && (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
                lineEdit->clear();
                view->hide();
                lineEdit->setText("aaaaaa");
                qDebug() << "CLEARED"
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
                QRect mainWindowGeometry = mainWindow->geometry();
                QRect screenGeometry = QApplication::desktop()->screenGeometry(mainWindow);
                int screenYPosition = mainWindowGeometry.y() - screenGeometry.y();

                int f_pos_y = globalPos.y() - screenYPosition;

                const int EXPAND_ZONE_HEIGHT = 22;
                
                // Workaround: If the mouses moves to fast then the menu bar is not gonna get show. So we change it from 0 to 2 pixels
                // TODO: Find a better solution. By instead moving the ccode inside of "(event->type() == QEvent::HoverMove)" to something that get called even if we are outside of the window on mouse movement.
                if (f_pos_y <= 2) {
                    if (!entered_zone) {
                        mainWindow->menuBar()->setFixedHeight(originalMenuBarHeight);
                        entered_zone = true;   
                    }
                } else if (f_pos_y > EXPAND_ZONE_HEIGHT) {
                    if (entered_zone) {
                        mainWindow->menuBar()->setFixedHeight(0);
                        entered_zone = false;   
                    }
                }
            }
            return false;
        }
    }
};

class QTextEditFilter : public QObject
{
    Q_OBJECT

signals:
    void chat_return_pressed();

public:
    bool text_edit_preserve_selection = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QTextEdit *textEdit = qobject_cast<QTextEdit *>(obj);

        if (textEdit != nullptr) {
            // Key press detection
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
                
                if (keyEvent != nullptr) {
                    if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) && 
                        (keyEvent->modifiers() & Qt::ShiftModifier)) {
                        // Add newline
                        QTextCursor cursor = textEdit->textCursor();
                        cursor.insertText("\\n");
                        // cursor.insertText("\n");
                        cursor.insertBlock();
                        cursor.movePosition(QTextCursor::StartOfBlock);
                        textEdit->setTextCursor(cursor);
                        return true;
                    } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                        emit chat_return_pressed();
                        return true;
                    }
                }
            }
            
            // Focus-out event handling
            if (event->type() == QEvent::FocusOut && text_edit_preserve_selection) {
                QTextCursor cursor = textEdit->textCursor();
                
                // We check if there's a selection
                if (cursor.hasSelection()) {
                    int start = cursor.selectionStart();
                    int len = cursor.selectionEnd() - start;
                    if (start != -1 && len != -1) {
                        cursor.setPosition(start);
                        cursor.setPosition(start + len, QTextCursor::KeepAnchor);
                        textEdit->setTextCursor(cursor);
                        return true;
                    }
                } else {
                    // We don't. Remove focus.
                    textEdit->clearFocus();
                }
            }
        }
        return false;
    }
};

#endif // EVENTFILTERS_H
