#include "aotextarea.h"

AOTextArea::AOTextArea(QWidget *parent)
    : AOTextArea(5000, parent)
{}

AOTextArea::AOTextArea(int maximumLogLenth, QWidget *parent)
    : QTextBrowser(parent)
{
  document()->setMaximumBlockCount(maximumLogLenth);
}

void AOTextArea::addMessage(QString name, QString message, QString nameColor, QString messageColor)
{
  const QTextCursor old_cursor = this->textCursor();
  const int old_scrollbar_value = this->verticalScrollBar()->value();
  const bool is_scrolled_down = old_scrollbar_value == this->verticalScrollBar()->maximum();

  this->moveCursor(QTextCursor::End);

  this->append("");
  if (!name.isEmpty())
  {
    this->insertHtml("<b><font color=" + nameColor + ">" + name.toHtmlEscaped() + "</font></b>:&nbsp;");

    // cheap workarounds ahoy
    message += " ";
  }

  // Detect URLs before HTML escaping to prevent & from becoming &amp; in links
  QString result;
  int lastEnd = 0;
  QRegularExpressionMatchIterator it = url_parser_regex.globalMatch(message);
  while (it.hasNext())
  {
    QRegularExpressionMatch match = it.next();
    result += message.mid(lastEnd, match.capturedStart() - lastEnd).toHtmlEscaped();
    QString url = match.captured(1);
    result += "<a href='" + url + "'>" + url.toHtmlEscaped() + "</a>";
    lastEnd = match.capturedEnd();
  }
  result += message.mid(lastEnd).toHtmlEscaped();
  result.replace("\n", "<br>");

  if (!messageColor.isEmpty())
  {
    result = "<font color=" + messageColor + ">" + result + "</font>";
  }

  this->insertHtml(result);

  this->auto_scroll(old_cursor, old_scrollbar_value, is_scrolled_down);
}

void AOTextArea::auto_scroll(QTextCursor old_cursor, int old_scrollbar_value, bool is_scrolled_down)
{
  if (old_cursor.hasSelection() || !is_scrolled_down)
  {
    // The user has selected text or scrolled away from the bottom: maintain
    // position.
    this->setTextCursor(old_cursor);
    this->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
    // The user hasn't selected any text and the scrollbar is at the bottom:
    // scroll to the bottom.
    this->moveCursor(QTextCursor::End);
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
  }
}
