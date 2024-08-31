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
    QFontMetrics metrics = QFontMetrics(this->font());
    QRect tr = metrics.boundingRect(text()).adjusted(0, 0, w, w);
    int l_indent;
    int x;
    int y;

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

    if (alignment() & Qt::AlignLeft)
    {
      x = rect.left() + l_indent - std::min(metrics.leftBearing(text()[0]), 0);
    }
    else if (alignment() & Qt::AlignRight)
    {
      x = rect.x() + rect.width() - l_indent - tr.width();
    }
    else
    {
      x = (rect.width() - tr.width()) / 2;
    }

    if (alignment() & Qt::AlignTop)
    {
      y = rect.top() + l_indent + metrics.ascent();
    }
    else if (alignment() & Qt::AlignBottom)
    {
      y = rect.y() + rect.height() - l_indent - metrics.descent();
    }
    else
    {
      y = (rect.height() + metrics.ascent() - metrics.descent()) / 2;
    }

    m_pen.setWidth(w * 2);
    QPainterPath path;
    path.addText(x, y, font(), text());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.strokePath(path, m_pen);
    if (1 < m_brush.style() && m_brush.style() < 15)
      painter.fillPath(path, palette().window());
    painter.fillPath(path, m_brush);
  }
  else
  {
    // Use the default renderer
    QLabel::paintEvent(event);
  }
}
