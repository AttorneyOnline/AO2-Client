#include "aotextarea.h"

#include <QScrollBar>
#include <QTextCursor>
#include <QRegExp>

AOTextArea::AOTextArea(QWidget *p_parent) : QTextBrowser(p_parent)
{

}

void AOTextArea::append_chatmessage(QString p_name, QString p_message)
{
  const QTextCursor old_cursor = this->textCursor();
  const int old_scrollbar_value = this->verticalScrollBar()->value();
  const bool is_scrolled_down = old_scrollbar_value == this->verticalScrollBar()->maximum();

  this->moveCursor(QTextCursor::End);

  this->append(p_name + ": ");

  QStringList word_list = p_message.split(" ");

  for (QString i_word : word_list)
  {
    if (i_word.startsWith("http"))
    {
      i_word.replace("\n", "").replace("\r", "");
      this->insertHtml("<a href=\"" + i_word + "\">" + i_word + "</a> ");
    }
    else
      this->insertPlainText(i_word + " ");
  }

  if (old_cursor.hasSelection() || !is_scrolled_down)
  {
      // The user has selected text or scrolled away from the bottom: maintain position.
      this->setTextCursor(old_cursor);
      this->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
      this->moveCursor(QTextCursor::End);
      this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
  }
}
