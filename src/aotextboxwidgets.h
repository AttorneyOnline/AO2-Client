#pragma once

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QTextEdit>

class AOChatboxLabel : public QLabel
{
  Q_OBJECT

public:
  AOChatboxLabel(QWidget *parent);

  void setIsOutlined(bool outlined);

  bool pointMode();
  void setPointMode(bool mode);

  double outlineThickness();
  void setOutlineThickness(double w);

  void setBrush(QBrush brush);
  void setPen(QPen pen);

  QSize sizeHint();
  QSize minimumSizeHint();

protected:
  void paintEvent(QPaintEvent *event);

private:
  bool m_outline = false;
  bool m_pointmode = false;
  int m_outline_width = 1;
  QBrush m_brush;
  QPen m_pen;
};
