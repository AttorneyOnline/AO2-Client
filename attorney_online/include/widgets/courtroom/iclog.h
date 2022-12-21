#ifndef ICLOG_H
#define ICLOG_H

#include <QFuture>
#include <QWidget>

class chatlogpiece;

class ICLog : QWidget {
public:
  explicit ICLog();
  virtual ~ICLog() = default;

  void setupUI();

  /**
   * @brief Meatgrinder function to append a message to the IC log.
   * @param p_name
   * @param p_showname
   * @param p_message
   * @param p_action
   * @param p_color
   * @param p_selfname
   */
  virtual void log_ic_text(QString p_name, QString p_showname,
                           QString p_message, QString p_action, int p_color,
                           bool p_selfname) = 0;

  /**
   * @brief Returns the history buffer.
   *
   * @return Returns the current IC log history
   */
  const QVector<chatlogpiece> getHistory() { return history; }

  /**
   * @brief Regenerates the IC log from the history buffer.
   *
   */
  virtual void regenerateLog(bool showname_enabled) = 0;

  /**
   * @brief Sets the history buffer and regeneratesd the IC log.
   * @param f_history
   * @param showname
   */
  void setHistory(const QVector<chatlogpiece> &f_history, const bool &showname)
  {
    history = f_history;
    regenerateLog(showname);
  };

private:
  /**
   * @brief Appends the latest log entry to history and trims old entries.
   * You'd want it to be a circular buffer, but this is not so easy.
   */
  virtual void appendToHistory(chatlogpiece history_entry) = 0;

  /**
   * @brief Stores every chatlog message up to the maximum buffer size.
   *
   * @details Used to recreate the entire iclog and replace the character name
   * with the showname or vise versa.
   */
  QVector<chatlogpiece> history;
};

#endif // ICLOG_H
