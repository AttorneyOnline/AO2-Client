#include "chatlogpiece.h"
#include "options.h"
#include "widgets/courtroom/downwards_iclog.h"

#include <QTextEdit>
#include <QUiLoader>

void DownwardICLog::setupUI() {}

void DownwardICLog::appendICText(QString p_text, QString p_name = "", QString action = "",
                                 int color = 0, bool selfname = false,
                                 QDateTime timestamp = QDateTime::currentDateTime(),
                                 bool ghost = false)
{
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

void DownwardICLog::setHistory(const QVector<chatlogpiece> &f_history,
                               const bool &showname)
{
  history = f_history;
  regenerateLog(showname);
}

void DownwardICLog::appendToHistory(chatlogpiece history_entry)
{
  if (history.size() > Options().maxLogSize() && !history.isEmpty()) {
    history.removeFirst();
  }
  history.append(history_entry);
}
