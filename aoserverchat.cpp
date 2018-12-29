#include "aoserverchat.h"
#include "aouiloader.h"

#include <QScrollBar>
#include <QVBoxLayout>

AOServerChat::AOServerChat(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/ooc.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_chatlog = findChild<QTextBrowser *>("chatlog");
  ui_message = findChild<QLineEdit *>("message");
  ui_name = findChild<QLineEdit *>("name");

  ui_name->setMaxLength(30);
  ui_name->setText(ao_app->get_default_username());

  connect(ui_message, &QLineEdit::returnPressed, this, &AOServerChat::send_message);
}

void AOServerChat::send_message()
{
  if (ui_name->text().isEmpty())
  {
    append_error("Please enter a name.");
    return;
  }

  emit messageSent(ui_name->text(), ui_message->text());
  emit ui_message->clear();
  ui_message->setFocus();
}

void AOServerChat::append_chat_message(QString name, QString message, QString color)
{
  CursorState old_state = get_scroll_state(ui_chatlog);
  ui_chatlog->moveCursor(QTextCursor::End);

  ui_chatlog->append("");
  ui_chatlog->insertHtml("<b><font color=" + color + ">" + name.toHtmlEscaped() + "</font></b>:&nbsp;");

  QString result = (message + " ").toHtmlEscaped().replace("\n", "<br>").replace(omnis_dank_url_regex, "<a href='\\1'>\\1</a>" );

  ui_chatlog->insertHtml(result);

  auto_scroll(old_state);
}

void AOServerChat::append_error(QString message)
{
  CursorState old_state = get_scroll_state(ui_chatlog);
  ui_chatlog->moveCursor(QTextCursor::End);

  ui_chatlog->append("");
  QString result = (message + " ").toHtmlEscaped().replace("\n", "<br>").replace(omnis_dank_url_regex, "<a href='\\1'>\\1</a>" );

  ui_chatlog->insertHtml("<font color='red'>" + result + "</font>");

  auto_scroll(old_state);
}

void AOServerChat::append_text(QString message)
{
  CursorState old_state = get_scroll_state(ui_chatlog);
  ui_chatlog->moveCursor(QTextCursor::End);

  ui_chatlog->append("");
  ui_chatlog->insertHtml(message);

  auto_scroll(old_state);
}

CursorState AOServerChat::get_scroll_state(QTextEdit *textArea)
{
  CursorState state;
  state.cursor = textArea->textCursor();
  state.scrollbar_value = textArea->verticalScrollBar()->value();
  state.is_scrolled_down = state.scrollbar_value == textArea->verticalScrollBar()->maximum();

  return state;
}

void AOServerChat::auto_scroll(CursorState &old_state)
{
  if (old_state.cursor.hasSelection() || !old_state.is_scrolled_down)
  {
      // The user has selected text or scrolled away from the bottom: maintain position.
      ui_chatlog->setTextCursor(old_state.cursor);
      ui_chatlog->verticalScrollBar()->setValue(old_state.scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
      ui_chatlog->moveCursor(QTextCursor::End);
      ui_chatlog->verticalScrollBar()->setValue(ui_chatlog->verticalScrollBar()->maximum());
  }
}
