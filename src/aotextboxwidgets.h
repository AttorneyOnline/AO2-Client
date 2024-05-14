#pragma once

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTextEdit>

class AOChatboxLabel : public QLabel
{
  Q_OBJECT

public:
  AOChatboxLabel(QWidget *parent);

  void setIsOutlined(bool outlined);
  void setOutlineColor(QColor color);
  void setOutlineWidth(int width);

  void setTextColor(QColor color);

protected:
  void paintEvent(QPaintEvent *event);

private:
  bool m_outline = false;
  QColor m_outline_color;
  int m_outline_width = 1;
  QColor m_text_color;
};
