#ifndef ICLOG_H
#define ICLOG_H

#include <QDateTime>
#include <QWidget>

class chatlogpiece;

class ICLog : public QWidget {
  Q_OBJECT
public:
  explicit ICLog();
  virtual ~ICLog() = default;

  /**
   * @brief Loads the widget and handles the creation of
   */
  virtual void setupUI() = 0;

  /**
   * @brief Regenerates the IC log from the history buffer.
   *
   */
  virtual void regenerateLog(bool showname_enabled) = 0;

  /**
   * @brief Appends the latest log entry to history and trims old entries.
   *
   */
  virtual void appendToHistory(chatlogpiece history_entry) = 0;

  /**
   * @brief Filters out the common CC inline text trickery, for appending to the
   * IC chatlog.
   * @param p_text
   * @param colorize
   * @param pos
   * @param default_color
   */
  virtual QString filterICText(QString p_text, bool colorize = false, int pos = -1,
                            int default_color = 0);

  /**
   * @brief Stores every chatlog message up to the maximum buffer size.
   *
   * @details Used to recreate the entire iclog and replace the character name
   * with the showname or vise versa.
   */
  QVector<chatlogpiece> history;

public slots:
  /**
   * @brief Closes the visible UI and redraws the widget with the new UI file.
   */
  virtual void reloadUI() = 0;

  /**
   * @brief Not to be confused with appendICText
   * @param p_name
   * @param p_showname
   * @param p_message
   * @param p_action
   * @param p_color
   * @param p_selfname
   */
  virtual void logICText(QString p_name, QString p_showname, QString p_message,
                         QString p_action = "", int p_color = 0,
                         bool p_selfname = false) = 0;

  /**
   * @brief Not to be confused with logICText
   * @param p_text
   * @param p_name
   * @param action
   * @param color
   * @param selfname
   * @param timestamp
   * @param ghost
   */
  virtual void appendICText(QString p_text, QString p_name = "",
                            QString action = "", int color = 0,
                            bool selfname = false,
                            QDateTime timestamp = QDateTime::currentDateTime(),
                            bool ghost = false) = 0;
};

#endif // ICLOG_H
