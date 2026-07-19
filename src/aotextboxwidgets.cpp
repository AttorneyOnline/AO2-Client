#include "aotextboxwidgets.h"

// Sane outlined QLabel solution ported from PyQt solution on StackOverflow by alec
// https://stackoverflow.com/questions/64290561/qlabel-correct-positioning-for-text-outline

AOChatboxLabel::AOChatboxLabel(QWidget *parent)
    : QLabel(parent)
{
  setBrush(QBrush(Qt::white));
  setPen(QPen(Qt::black));
}

void AOChatboxLabel::setIsOutlined(bool outlined)
{
  m_outline = outlined;
}

bool AOChatboxLabel::pointMode()
{
  return m_pointmode;
}

void AOChatboxLabel::setPointMode(bool mode)
{
  m_pointmode = mode;
}

double AOChatboxLabel::outlineThickness()
{
  if (pointMode())
  {
    return m_outline_width * font().pointSize();
  }
  else
    return m_outline_width;
}

void AOChatboxLabel::setOutlineThickness(double w)
{
  m_outline_width = w;
}

void AOChatboxLabel::setBrush(QBrush brush)
{
  m_brush = brush;
}
void AOChatboxLabel::setPen(QPen pen)
{
  m_pen = pen;
}

QSize AOChatboxLabel::sizeHint()
{
  int nrml_w = std::ceil(outlineThickness() * 2);
  return QLabel::sizeHint() + QSize(nrml_w, nrml_w);
}
QSize AOChatboxLabel::minimumSizeHint()
{
  int nrml_w = std::ceil(outlineThickness() * 2);
  return QLabel::minimumSizeHint() + QSize(nrml_w, nrml_w);
}

void AOChatboxLabel::paintEvent(QPaintEvent *event)
{
  if (m_outline)
  {
    double w = outlineThickness();
    QRectF rect = this->rect();
    QFontMetrics metrics(this->font());
    int l_indent;
    qreal x;
    qreal y;

    if (indent() == -1)
    {
      if (frameWidth())
      {
        l_indent = (metrics.boundingRect("x").width() + w * 2) / 2;
      }
      else
      {
        l_indent = w;
      }
    }
    else
    {
      l_indent = indent();
    }

    QPainterPath path;
    path.addText(0, 0, font(), text());
    QRectF tr = path.boundingRect().adjusted(-w, -w, w, w);

    if (alignment() & Qt::AlignLeft)
    {
      x = rect.left() + l_indent - tr.left();
    }
    else if (alignment() & Qt::AlignRight)
    {
      x = rect.right() - l_indent - tr.right();
    }
    else
    {
      x = rect.left() + (rect.width() - tr.width()) / 2.0 - tr.left();
    }

    if (alignment() & Qt::AlignTop)
    {
      y = rect.top() + l_indent - tr.top();
    }
    else if (alignment() & Qt::AlignBottom)
    {
      y = rect.bottom() - l_indent - tr.bottom();
    }
    else
    {
      y = rect.top() + (rect.height() - tr.height()) / 2.0 - tr.top();
    }

    m_pen.setWidth(w * 2);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(x, y);
    painter.strokePath(path, m_pen);
    if (1 < m_brush.style() && m_brush.style() < 15)
    {
      painter.fillPath(path, palette().window());
    }
    painter.fillPath(path, m_brush);
  }
  else
  {
    // Use the default renderer
    QLabel::paintEvent(event);
  }
}
