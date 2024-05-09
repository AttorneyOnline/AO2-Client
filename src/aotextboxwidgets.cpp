#include "aotextboxwidgets.h"

AOChatboxLabel::AOChatboxLabel(QWidget *parent) : QLabel(parent) {}

void AOChatboxLabel::paintEvent(QPaintEvent *event)
{
  if (is_outlined) {
    QBrush brush;
    QPen pen;
    QPointF baseline(outline_width, fontMetrics().height());

    // Set up brush (base text)
    brush.setColor(text_color);
    brush.setStyle(Qt::SolidPattern);

    // Set up outline
    pen.setColor(outline_color);
    pen.setWidthF(outline_width);

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
  else {
    // Use the default renderer
    QLabel::paintEvent(event);
  }
}
