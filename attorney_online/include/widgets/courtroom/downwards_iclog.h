#ifndef DOWNWARD_ICLOG_H
#define DOWNWARD_ICLOG_H

#include "widgets/courtroom/iclog.h"

class QTextEdit;

class DownwardICLog : public ICLog {
public:
  DownwardICLog() = default;
  ~DownwardICLog();

  void setupUI() override;

  void appendICText(QString p_text, QString p_name,
                    QString action,
                    int color, bool selfname, QDateTime timestamp, bool ghost) override;

  /**
   * @brief Regenerates the IC log from the history buffer.
   */
  void regenerateLog(bool showname_enabled) override;

public slots:
  void reloadUI() override;

private:
  QTextEdit *ui_ic_chatlog;

  /**
   * @brief Appends the latest log entry to history and trims old entries.
   */
  void appendToHistory(chatlogpiece history_entry) override;
};

#endif // DOWNWARD_ICLOG_H
