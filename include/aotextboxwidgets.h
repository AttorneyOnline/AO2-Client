#ifndef AOTEXTBOXWIDGETS_H
#define AOTEXTBOXWIDGETS_H

#include <QDebug>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTextEdit>

class AOChatboxLabel : public QLabel {
  Q_OBJECT

public:
  AOChatboxLabel(QWidget *parent);
  void paintEvent(QPaintEvent *event);

  void setOutlineColor(QColor color) { outline_color = color; };
  void setOutlineWidth(int width) { outline_width = width; };
  void setTextColor(QColor color) { text_color = color; };
  void setIsOutlined(bool outlined) { is_outlined = outlined; };

protected:
private:
  QColor outline_color;
  QColor text_color;
  int outline_width = 1;
  bool is_outlined = false;
};

class AOChatboxTextDocument : public QTextDocument {
  Q_OBJECT

public:
  AOChatboxTextDocument(QWidget *parent);
  void drawContents(QPainter *painter);

  void setCursor(QTextCursor newcursor) { cursor = newcursor; };

  void setOutlineColor(QColor color) { outline_color = color; };
  void setOutlineWidth(int width) { outline_width = width; };
  void setTextColor(QColor color) { text_color = color; };
  void setIsOutlined(bool outlined) { is_outlined = outlined; };

private:
  QColor outline_color;
  QColor text_color;
  int outline_width = 1;
  bool is_outlined = false;
  QTextCursor cursor;
  QWidget *m_parent;
};

class AOChatboxTextEdit : public QTextEdit {
  Q_OBJECT

public:
  AOChatboxTextEdit(QWidget *parent);
  void paintEvent(QPaintEvent *event);

  void setOutlineColor(QColor color) { outline_color = color; };
  void setOutlineWidth(int width) { outline_width = width; };
  void setTextColor(QColor color) { text_color = color; };
  void setIsOutlined(bool outlined) { is_outlined = outlined; };

protected:
private:
  QColor outline_color;
  QColor text_color;
  int outline_width = 1;
  bool is_outlined = false;
};

#endif // AOTEXTBOXWIDGETS_H
