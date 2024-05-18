#include "aotextboxwidgets.h"

AOChatboxLabel::AOChatboxLabel(QWidget *parent)
    : QLabel(parent)
{}

void AOChatboxLabel::setOutlineColor(QColor color)
{
  m_outline_color = color;
}

void AOChatboxLabel::setOutlineWidth(int width)
{
  m_outline_width = width;
}

void AOChatboxLabel::setIsOutlined(bool outlined)
{
  m_outline = outlined;
}

void AOChatboxLabel::setTextColor(QColor color)
{
  m_text_color = color;
}

void AOChatboxLabel::paintEvent(QPaintEvent *event)
{
  if (m_outline)
  {
    QBrush brush;
    QPen pen;
    QPointF baseline(m_outline_width, fontMetrics().height());

    // Set up brush (base text)
    brush.setColor(m_text_color);
    brush.setStyle(Qt::SolidPattern);

    // Set up outline
    pen.setColor(m_outline_color);
    pen.setWidthF(m_outline_width);

    QPainterPath path;
    path.addText(baseline, font(), text());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // draw outline
    painter.setPen(pen);
    painter.drawPath(path);
    // remove outline pen, then draw text on top
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPath(path);
  }
  else
  {
    // Use the default renderer
    QLabel::paintEvent(event);
  }
}
