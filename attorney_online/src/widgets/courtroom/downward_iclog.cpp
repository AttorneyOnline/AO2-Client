#include "chatlogpiece.h"
#include "options.h"
#include "widgets/courtroom/downwards_iclog.h"

#include <QTextEdit>
#include <QScrollBar>
#include <QUiLoader>
#include <QFile>
#include <QDebug>

void DownwardICLog::setupUI() {
    QUiLoader(this);

    QFile l_chatlog_ui("");

    // Auto-cull oversized ic-log
    ui_ic_chatlog->document()->setMaximumBlockCount(options.maxLogSize());
}

void DownwardICLog::appendICText(QString p_text, QString p_name = "", QString action = "",
                                 int color = 0, bool selfname = false,
                                 QDateTime timestamp = QDateTime::currentDateTime(),
                                 bool ghost = false)
{
    QColor chatlog_color = ao_app->get_color("ic_chatlog_color", "courtroom_fonts.ini");
    QTextCharFormat bold;
    QTextCharFormat normal;
    QTextCharFormat italics;
    QTextCharFormat own_name;
    QTextCharFormat other_name;
    QTextCharFormat timestamp_format;
    QTextCharFormat selftimestamp_format;
    QTextBlockFormat format;
    bold.setFontWeight(QFont::Bold);
    normal.setFontWeight(QFont::Normal);
    italics.setFontItalic(true);
    own_name.setFontWeight(QFont::Bold);
    own_name.setForeground(ao_app->get_color("ic_chatlog_selfname_color", "courtroom_fonts.ini"));
    other_name.setFontWeight(QFont::Bold);
    other_name.setForeground(ao_app->get_color("ic_chatlog_showname_color", "courtroom_fonts.ini"));
    timestamp_format.setForeground(ao_app->get_color("ic_chatlog_timestamp_color", "courtroom_fonts.ini"));
    selftimestamp_format.setForeground(ao_app->get_color("ic_chatlog_selftimestamp_color", "courtroom_fonts.ini"));
    format.setTopMargin(log_margin);
    const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
    const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();
    const bool need_newline = !ui_ic_chatlog->document()->isEmpty();
    const int scrollbar_target_value =
        log_goes_downwards ? ui_ic_chatlog->verticalScrollBar()->maximum()
                           : ui_ic_chatlog->verticalScrollBar()->minimum();

    if (ghost) {
      ghost_blocks++;
      chatlog_color.setAlpha(128);
      bold.setForeground(chatlog_color);
      normal.setForeground(chatlog_color);
      italics.setForeground(chatlog_color);
    }
    else {
      last_ic_message = p_name + ":" + p_text;
    }

    ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::End
                                                 : QTextCursor::Start);

    if (!ghost && ghost_blocks > 0) {
      for (int i = 0; i < ghost_blocks; ++i) {
        ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::PreviousBlock
                                                     : QTextCursor::NextBlock);
      }
      ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::EndOfBlock
                                                   : QTextCursor::StartOfBlock);
    }

    // Only prepend with newline if log goes downwards
    if (log_goes_downwards && need_newline) {
      ui_ic_chatlog->textCursor().insertBlock(format);
    }


    // Timestamp if we're doing that meme
    if (options.logTimestampEnabled()) {
      // Format the timestamp
      QTextCharFormat format = selfname ? selftimestamp_format : timestamp_format;
      if (timestamp.isValid()) {
        ui_ic_chatlog->textCursor().insertText(
          "[" + timestamp.toString(options.logTimestampFormat()) + "] ", format);
      } else {
        qCritical() << "could not insert invalid timestamp" << timestamp;
      }
    }

    // Format the name of the actor
    QTextCharFormat name_format = selfname ? own_name : other_name;
    ui_ic_chatlog->textCursor().insertText(p_name, name_format);
    // Special case for stopping the music
    if (p_action == tr("has stopped the music")) {
      ui_ic_chatlog->textCursor().insertText(" " + p_action + ".", normal);
    }
    // Make shout text bold
    else if (p_action == tr("shouts") && log_ic_actions) {
      ui_ic_chatlog->textCursor().insertText(" " + p_action + " ", normal);
      if (log_colors && !ghost) {
        ui_ic_chatlog->textCursor().insertHtml(
            "<b>" +
            filter_ic_text(p_text, true, -1, 0)
                .replace(
                    "$c0",
                    chatlog_color.name(QColor::HexArgb)) +
            "</b>");
      }
      else
        ui_ic_chatlog->textCursor().insertText(" " + p_text, italics);
    }
    // If action not blank:
    else if (p_action != "" && log_ic_actions) {
      // Format the action in normal
      ui_ic_chatlog->textCursor().insertText(" " + p_action, normal);
      if (log_newline)
          // For some reason, we're forced to use <br> instead of the more sensible
          // \n. Why? Because \n is treated as a new Block instead of a soft newline
        // within a paragraph!
        ui_ic_chatlog->textCursor().insertHtml("<br>");
      else
        ui_ic_chatlog->textCursor().insertText(": ", normal);
      // Format the result in italics
      ui_ic_chatlog->textCursor().insertText(p_text + ".", italics);
    }
    else {
      if (log_newline)
        // For some reason, we're forced to use <br> instead of the more sensible
        // \n. Why? Because \n is treated as a new Block instead of a soft newline
        // within a paragraph!
        ui_ic_chatlog->textCursor().insertHtml("<br>");
      else
        ui_ic_chatlog->textCursor().insertText(": ", normal);
      // Format the result according to html
      if (log_colors) {
        QString p_text_filtered = filter_ic_text(p_text, true, -1, color);
        p_text_filtered = p_text_filtered.replace("$c0", chatlog_color.name(QColor::HexArgb));
        for (int c = 1; c < max_colors; ++c) {
          QColor color_result = default_color_rgb_list.at(c);
          if (ghost) {
            color_result.setAlpha(128);
          }
          p_text_filtered = p_text_filtered.replace("$c" + QString::number(c), color_result.name(QColor::HexArgb));
        }
        ui_ic_chatlog->textCursor().insertHtml(p_text_filtered);
      }
      else
        ui_ic_chatlog->textCursor().insertText(filter_ic_text(p_text, false), normal);
    }

    // Only append with newline if log goes upwards
    if (!log_goes_downwards && need_newline) {
      ui_ic_chatlog->textCursor().insertBlock(format);
    }

    // If we got too many blocks in the current log, delete some.
    while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks &&
           log_maximum_blocks > 0) {
      QTextCursor temp_curs = ui_ic_chatlog->textCursor();
      temp_curs.movePosition(log_goes_downwards ? QTextCursor::Start
                                                   : QTextCursor::End);
      temp_curs.select(QTextCursor::BlockUnderCursor);
      temp_curs.removeSelectedText();
      if (log_goes_downwards)
        temp_curs.deleteChar();
      else
        temp_curs.deletePreviousChar();
    }

    // Finally, scroll the scrollbar to the correct position.
    if (old_cursor.hasSelection() ||
        old_scrollbar_value != scrollbar_target_value) {
      // The user has selected text or scrolled away from the bottom: maintain
      // position.
      ui_ic_chatlog->setTextCursor(old_cursor);
      ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
    }
    else {
      ui_ic_chatlog->verticalScrollBar()->setValue(
          log_goes_downwards ? ui_ic_chatlog->verticalScrollBar()->maximum() : 0);
    }
}

void DownwardICLog::regenerateLog(bool showname_enabled)
{
  ui_ic_chatlog->clear();
  foreach (chatlogpiece item, history) {
    QString message = item.get_message();
    QString name = showname_enabled ? item.get_showname() : item.get_name();
    appendICText(message, name, item.get_action(), item.get_chat_color(),
                 item.get_selfname(), item.get_datetime().toLocalTime());
  }
}

void DownwardICLog::reloadUI()
{

}

void DownwardICLog::logICText(QString p_name, QString p_showname, QString p_message, QString p_action, int p_color, bool p_selfname)
{
    if (options.logToTextFileEnabled())
        emit appendToFile(log_entry.get_full());
      ao_app->append_to_file(log_entry.get_full(), ao_app->log_filename, true);

    while (ic_chatlog_history.size() > log_maximum_blocks &&
           log_maximum_blocks > 0) {
      ic_chatlog_history.removeFirst();
    }
}

void DownwardICLog::appendToHistory(chatlogpiece history_entry)
{
  if (history.size() > Options().maxLogSize() && !history.isEmpty()) {
    history.removeFirst();
  }
  history.append(history_entry);
}
