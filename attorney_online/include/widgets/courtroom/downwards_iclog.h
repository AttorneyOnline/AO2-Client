#ifndef DOWNWARD_ICLOG_H
#define DOWNWARD_ICLOG_H

#include "widgets/courtroom/iclog.h"
#include "options.h"

class QTextEdit;
class QWidget;

class DownwardICLog : public ICLog {
    Q_OBJECT
public:
  DownwardICLog() = default;
  ~DownwardICLog();

  void setupUI() override;

  void regenerateLog(bool showname_enabled) override;

public slots:
  void reloadUI() override;

  void logICText(QString p_name, QString p_showname, QString p_message,
                         QString p_action = "", int p_color = 0,
                         bool p_selfname = false) override;

  void appendICText(QString p_text, QString p_name,
                    QString action,
                    int color, bool selfname, QDateTime timestamp, bool ghost) override;
signals:
  /**
   * @brief Emitted when a log entry is to be appended into the non-demo plain-text log.
   */
  void appendToFile(QString p_message);
private:
  QTextEdit *ui_ic_chatlog;

  QWidget *ui_chatlog_widget;

  //Idk what this is used for.
  int ghost_blocks = 0;

  /**
   * @brief Appends the latest log entry to history and trims old entries.
   */
  void appendToHistory(chatlogpiece history_entry) override;

  void filterICText(QString p_text, bool colorize = false, int pos = -1,
                    int default_color = 0) override;

  Options options;
};

#endif // DOWNWARD_ICLOG_H
