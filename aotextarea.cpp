#include "aotextarea.h"

#include <QScrollBar>
#include <QTextCursor>
#include <QRegExp>
#include <QDebug>

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

  QRegExp regExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");

  QString result = p_message.toHtmlEscaped().replace("\n", "<br>").replace(regExp, "<a href='\\1'>\\1</a>" );

  this->insertHtml(result);


  /*
  QRegExp rx("\\bhttp://\\S+");

  int first_index = rx.indexIn(p_message);

  qDebug() << "number of rx indices: " << rx.captureCount();

  if (first_index < 0)
  {
    this->insertPlainText(p_message);
    qDebug() << "NO REGEX MATCHES";
    return;
  }

  //indices where we found a regex match
  QVector<int> rx_indices;
  QStringList links = rx.capturedTexts();

  qDebug() << "link size" << links.size();

  rx_indices.append(first_index);


  //start at one because first_index is already appended
  for (int n_pos = 1 ; n_pos < rx.captureCount() ; ++n_pos)
    rx_indices.append(rx.indexIn(p_message));

  for (int msg_pos = 0 ; msg_pos < p_message.size() ; ++msg_pos)
  {
    int tag_index = rx_indices.indexOf(msg_pos);
    if (tag_index < 0)
    {
      this->insertPlainText(p_message.at(msg_pos));
      continue;
    }

    QString link = links.at(tag_index);
    QString html_string = "<a href=\"" + link + "\">" + link + "</a>";
    qDebug() << "html: " << html_string;

    this->insertHtml(html_string);

    msg_pos += link.size() - 1;

  }

  */

  /*

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
  */

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
