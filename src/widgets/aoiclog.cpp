#include "widgets/aoiclog.h"
#include "aouiloader.h"
#include "widgets/courtroom.h"

#include <stack>

#include <QScrollBar>
#include <QVBoxLayout>

AOICLog::AOICLog(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/ic_log.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_ic_chatlog = findChild<QTextEdit *>("ui_ic_chatlog");
  ui_ic_chatlog->setReadOnly(true);
}

void AOICLog::append_ic_text(QString p_default_showname, QString p_showname,
                             QString p_text, bool is_songchange)
{
  int log_maximum_blocks = options.maxLogLines();

  // Append and purge chat log history
  chatlogpiece temp(p_default_showname, p_showname, p_text, is_songchange);
  ic_chatlog_history.append(temp);

  while(ic_chatlog_history.size() > log_maximum_blocks && log_maximum_blocks > 0)
  {
    ic_chatlog_history.removeFirst();
  }

  QTextCharFormat bold;
  QTextCharFormat normal;
  QTextCharFormat italics;
  bold.setFontWeight(QFont::Bold);
  normal.setFontWeight(QFont::Normal);
  italics.setFontItalic(true);
  const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
  const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();

  if (!is_songchange)
    p_text = filter_ic_text(p_text);

  if (!options.legacyScrollEnabled())
  {
      const bool is_scrolled_down = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->maximum();

      ui_ic_chatlog->moveCursor(QTextCursor::End);

      if (ui_ic_chatlog->toPlainText().size() != 0)
      {
        ui_ic_chatlog->textCursor().insertText("\n", bold);
      }

      ui_ic_chatlog->textCursor().insertText(p_showname, bold);

      if (is_songchange)
      {
        ui_ic_chatlog->textCursor().insertText(" has played a song: ", normal);
        ui_ic_chatlog->textCursor().insertText(p_text + ".", italics);
      }
      else
      {
        ui_ic_chatlog->textCursor().insertText(p_text, normal);
      }

      // If we got too many blocks in the current log, delete some from the top.
      while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks && log_maximum_blocks > 0)
      {
          ui_ic_chatlog->moveCursor(QTextCursor::Start);
          ui_ic_chatlog->textCursor().select(QTextCursor::BlockUnderCursor);
          ui_ic_chatlog->textCursor().removeSelectedText();
          ui_ic_chatlog->textCursor().deleteChar();
          //qDebug() << ui_ic_chatlog->document()->blockCount() << " < " << log_maximum_blocks;
      }

      if (old_cursor.hasSelection() || !is_scrolled_down)
      {
          // The user has selected text or scrolled away from the bottom: maintain position.
          ui_ic_chatlog->setTextCursor(old_cursor);
          ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
      }
      else
      {
          // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
          ui_ic_chatlog->moveCursor(QTextCursor::End);
          ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->maximum());
      }
  }
  else
  {
      const bool is_scrolled_up = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->minimum();

      ui_ic_chatlog->moveCursor(QTextCursor::Start);

      ui_ic_chatlog->textCursor().insertText(p_showname, bold);

      if (is_songchange)
      {
        ui_ic_chatlog->textCursor().insertText(" has played a song: ", normal);
        ui_ic_chatlog->textCursor().insertText(p_text + "." + '\n', italics);
      }
      else
      {
        ui_ic_chatlog->textCursor().insertText(p_text + '\n', normal);
      }

      // If we got too many blocks in the current log, delete some from the bottom.
      while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks && log_maximum_blocks > 0)
      {
          ui_ic_chatlog->moveCursor(QTextCursor::End);
          ui_ic_chatlog->textCursor().select(QTextCursor::BlockUnderCursor);
          ui_ic_chatlog->textCursor().removeSelectedText();
          ui_ic_chatlog->textCursor().deletePreviousChar();
          //qDebug() << ui_ic_chatlog->document()->blockCount() << " < " << log_maximum_blocks;
      }

      if (old_cursor.hasSelection() || !is_scrolled_up)
      {
          // The user has selected text or scrolled away from the top: maintain position.
          ui_ic_chatlog->setTextCursor(old_cursor);
          ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
      }
      else
      {
          // The user hasn't selected any text and the scrollbar is at the top: scroll to the top.
          ui_ic_chatlog->moveCursor(QTextCursor::Start);
          ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->minimum());
      }
  }
}

void AOICLog::reload()
{
  ui_ic_chatlog->clear();
  for (chatlogpiece &item : ic_chatlog_history) {
    append_ic_text(item.get_name(), item.get_showname(), item.get_message(),
                   item.get_is_song());
  }
}

void AOICLog::export_to_file(QFile &file)
{
  QTextStream out(&file);

  foreach (chatlogpiece item, ic_chatlog_history) {
    out << item.get_full() << '\n';
  }

  file.close();
}

QString AOICLog::filter_ic_text(QString p_text)
{
  // Get rid of centering.
  if(p_text.startsWith(": ~~"))
  {
      // Don't forget, the p_text part actually everything after the name!
      // Hence why we check for ': ~~'.

      // Let's remove those two tildes, then.
      // : _ ~ ~
      // 0 1 2 3
      p_text.remove(2,2);
  }

  // Get rid of the inline-colouring.
  // I know, I know, excessive code duplication.
  // Nobody looks in here, I'm fine.
  int trick_check_pos = 0;
  bool ic_next_is_not_special = false;
  QString f_character = p_text.at(trick_check_pos);
  std::stack<INLINE_COLOURS> ic_colour_stack;
  while (trick_check_pos < p_text.size())
  {
      f_character = p_text.at(trick_check_pos);

      // Escape character.
      if (f_character == "\\" and !ic_next_is_not_special)
      {
          ic_next_is_not_special = true;
          p_text.remove(trick_check_pos,1);
      }

      // Text speed modifier.
      else if (f_character == "{" and !ic_next_is_not_special)
      {
          p_text.remove(trick_check_pos,1);
      }
      else if (f_character == "}" and !ic_next_is_not_special)
      {
          p_text.remove(trick_check_pos,1);
      }

      // Orange inline colourisation.
      else if (f_character == "|" and !ic_next_is_not_special)
      {
          if (!ic_colour_stack.empty())
          {
              if (ic_colour_stack.top() == INLINE_ORANGE)
              {
                  ic_colour_stack.pop();
                  p_text.remove(trick_check_pos,1);
              }
              else
              {
                  ic_colour_stack.push(INLINE_ORANGE);
                  p_text.remove(trick_check_pos,1);
              }
          }
          else
          {
              ic_colour_stack.push(INLINE_ORANGE);
              p_text.remove(trick_check_pos,1);
          }
      }

      // Blue inline colourisation.
      else if (f_character == "(" and !ic_next_is_not_special)
      {
          ic_colour_stack.push(INLINE_BLUE);
          trick_check_pos++;
      }
      else if (f_character == ")" and !ic_next_is_not_special
               and !ic_colour_stack.empty())
      {
          if (ic_colour_stack.top() == INLINE_BLUE)
          {
              ic_colour_stack.pop();
              trick_check_pos++;
          }
          else
          {
              ic_next_is_not_special = true;
          }
      }

      // Grey inline colourisation.
      else if (f_character == "[" and !ic_next_is_not_special)
      {
          ic_colour_stack.push(INLINE_GREY);
          trick_check_pos++;
      }
      else if (f_character == "]" and !ic_next_is_not_special
               and !ic_colour_stack.empty())
      {
          if (ic_colour_stack.top() == INLINE_GREY)
          {
              ic_colour_stack.pop();
              trick_check_pos++;
          }
          else
          {
              ic_next_is_not_special = true;
          }
      }

      // Green inline colourisation.
      else if (f_character == "`" and !ic_next_is_not_special)
      {
          if (!ic_colour_stack.empty())
          {
              if (ic_colour_stack.top() == INLINE_GREEN)
              {
                  ic_colour_stack.pop();
                  p_text.remove(trick_check_pos,1);
              }
              else
              {
                  ic_colour_stack.push(INLINE_GREEN);
                  p_text.remove(trick_check_pos,1);
              }
          }
          else
          {
              ic_colour_stack.push(INLINE_GREEN);
              p_text.remove(trick_check_pos,1);
          }
      }
      else
      {
          trick_check_pos++;
          ic_next_is_not_special = false;
      }
  }

  return p_text;
}
