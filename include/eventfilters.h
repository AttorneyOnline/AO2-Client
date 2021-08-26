#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include <QEvent>
#include <QPlainTextEdit>
#include <QLineEdit>

class AOTextEditFilter : public QObject
{
    Q_OBJECT

protected:
    // QPlainTextEdit's viewport is its own object and it receives mouse events instead of the main object
    // so we are actually intercepting events meant for the viewport and working w/ its parent (the object we want) instead
    //
    // this took too long to figure out. QPlainTextEdit more like QPainTextEdit
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::MouseButtonDblClick && obj->parent() != nullptr) {
            QPlainTextEdit *textEdit = qobject_cast<QPlainTextEdit *>(obj->parent());
            if (textEdit != nullptr)
                textEdit->setReadOnly(false);
            return true;
        }
        return false;
    }
};

class AOLineEditFilter : public QObject
{
    Q_OBJECT
public:
    bool preserve_selection = false;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
        if (event->type() == QEvent::MouseButtonDblClick) // double click
            emit double_clicked();
        else if (event->type() == QEvent::FocusOut && lineEdit != nullptr && preserve_selection) { // lost focus
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



#endif // EVENTFILTERS_H
