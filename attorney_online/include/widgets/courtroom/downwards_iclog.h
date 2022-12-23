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
   *
   */
  void regenerateLog(bool showname_enabled) override;

  /**
   * @brief Sets the history buffer and regeneratesd the IC log.
   * @param f_history
   * @param showname
   *
   * @details You may wonder why this function exists. This is partially to
   * handle theme reloading, as it was not a consideration in the original code,
   * at least not from what I can tell.
   */
  void setHistory(const QVector<chatlogpiece> &f_history,
                  const bool &showname) override;

private:
  /**
   * @brief Appends the latest log entry to history and trims old entries.
   */
  void appendToHistory(chatlogpiece history_entry) override;

private:
  QTextEdit *ui_ic_chatlog;
};

#endif // DOWNWARD_ICLOG_H
