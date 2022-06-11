#include "aotextarea.h"

AOTextArea::AOTextArea(QWidget *p_parent) : QTextBrowser(p_parent)
{
  this->document()->setMaximumBlockCount(10);
}

void AOTextArea::append_linked(QString p_message)
{
  QString result = p_message.toHtmlEscaped()
                       .replace("\n", "<br>")
                       .replace(url_parser_regex, "<a href='\\1'>\\1</a>");
  this->insertHtml(result);
}

void AOTextArea::append_chatmessage(QString p_name, QString p_message,
                                    QString p_name_colour, QString p_color)
{
  const QTextCursor old_cursor = this->textCursor();
  const int old_scrollbar_value = this->verticalScrollBar()->value();
  const bool is_scrolled_down =
      old_scrollbar_value == this->verticalScrollBar()->maximum();

  this->moveCursor(QTextCursor::End);

  this->append("");
  if (!p_name.isEmpty()) {
    this->insertHtml("<b><font color=" + p_name_colour + ">" +
                     p_name.toHtmlEscaped() + "</font></b>:&nbsp;");

    // cheap workarounds ahoy
    p_message += " ";
  }

  QString result = p_message.toHtmlEscaped()
                       .replace("\n", "<br>")
                       .replace(url_parser_regex, "<a href='\\1'>\\1</a>");

  if (!p_color.isEmpty()) {
    result = "<font color=" + p_color + ">" + result + "</font>";
  }

  this->insertHtml(result);

  this->auto_scroll(old_cursor, old_scrollbar_value, is_scrolled_down);
  qDebug() << "Linecount : " << this->document()->lineCount() << Qt::endl
           << "Blockcount : " << this->document()->blockCount();
}

void AOTextArea::append_error(QString p_message)
{
  const QTextCursor old_cursor = this->textCursor();
  const int old_scrollbar_value = this->verticalScrollBar()->value();
  const bool is_scrolled_down =
      old_scrollbar_value == this->verticalScrollBar()->maximum();

  this->moveCursor(QTextCursor::End);

  this->append("");

  p_message += " ";
  QString result = p_message.replace("\n", "<br>")
                       .replace(url_parser_regex, "<a href='\\1'>\\1</a>");

  this->insertHtml("<font color='red'>" + result + "</font>");

  this->auto_scroll(old_cursor, old_scrollbar_value, is_scrolled_down);
}

void AOTextArea::auto_scroll(QTextCursor old_cursor, int old_scrollbar_value,
                             bool is_scrolled_down)
{
  if (old_cursor.hasSelection() || !is_scrolled_down) {
    // The user has selected text or scrolled away from the bottom: maintain
    // position.
    this->setTextCursor(old_cursor);
    this->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else {
    // The user hasn't selected any text and the scrollbar is at the bottom:
    // scroll to the bottom.
    this->moveCursor(QTextCursor::End);
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
  }
}
