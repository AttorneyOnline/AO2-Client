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

AOChatboxTextDocument::AOChatboxTextDocument(QWidget *parent)
    : QTextDocument(parent)
{
  m_parent = parent;
}

void AOChatboxTextDocument::drawContents(QPainter *painter)
{
  // placeholder values
  outline_color = QColor(255, 255, 255);
  text_color = QColor(0, 0, 0);
  outline_width = 4;

  QTextDocument::drawContents(painter);

  AOChatboxTextEdit *myParent = dynamic_cast<AOChatboxTextEdit *>(m_parent);

  QTextCursor myCursor = myParent->textCursor();
  QTextCharFormat myFormat = cursor.charFormat();

  myFormat.setTextOutline(QPen(outline_color, outline_width));
  myCursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
  myCursor.mergeCharFormat(myFormat);

  QTextDocument::drawContents(painter);

  myFormat.setTextOutline(Qt::NoPen);
  myCursor.mergeCharFormat(myFormat);

  QTextDocument::drawContents(painter);
}

AOChatboxTextEdit::AOChatboxTextEdit(QWidget *parent) : QTextEdit(parent)
{
  AOChatboxTextDocument *newdocument = new AOChatboxTextDocument(this);
  setDocument(newdocument);
}

void AOChatboxTextEdit::paintEvent(QPaintEvent *event)
{
  QPainter *painter;
  QTextEdit::paintEvent(event);

  document()->drawContents(painter);
}
