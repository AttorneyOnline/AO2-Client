#ifndef AOEVIDENCE_H
#define AOEVIDENCE_H

#include "aoapplication.h"

#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>

class AOEvidence : public QWidget
{
  Q_OBJECT

  /* Some behavior specifications for this window:
   *
   * 1. When evidence is clicked, the information about the
   *    evidence is shown on the right pane.
   * 2. When the "New Evidence" button is double-clicked, a
   *    new evidence item is created with default values and focus switches
   *    to the new evidence item. (Unfortunately, without good async
   *    support, the user cannot be notified if this was unsuccessful.)
   * 3. The "New Evidence" button in itself is not presentable or
   *    viewable and is treated as if no evidence item was selected.
   * 4. When evidence is edited by the user, the evidence is
   *    updated within a certain interval of time.
   * 5. When a call to set a new evidence list is received, existing
   *    evidence items are modified rather than replaced, in order to
   *    maintain the focus on an evidence item currently being modified.
   *    Extraneous items that exceed the new list's length are removed.
   *    An item currently being edited by the user will not be modified
   *    in order to maintain the consistency of what the user is currently
   *    typing.
   * 6. When evidence is deleted by the user, the client will wait for the
   *    server to echo back the updated list.
   */
public:
  explicit AOEvidence(QWidget *parent, AOApplication *p_ao_app);
  bool presenting();
  int selectedEvidenceId();
  void showEvidence(int id);
  void addMessageData(chat_message_type &message);

signals:
  void evidenceAdded(const evi_type &item);
  void evidenceEdited(int id, const evi_type &item);
  void evidenceDeleted(int id);

private:
  AOApplication *ao_app;
  QVector<evi_type> evidence;

  static const int debounceInterval = 500;
  QPointer<QTimer> debounceTimer;

  // The cause of the debounce will not be modified when the server
  // echoes back changes.
  QPointer<QWidget> debounceCause;

  QPlainTextEdit *ui_description;
  QLabel *ui_image;
  QTextEdit *ui_name;

  QListWidget *ui_list;
  QPushButton *ui_present_button;
  QPushButton *ui_delete_button;

  QStackedWidget *ui_pages;
  QWidget *ui_evidence_info_page;
  QWidget *ui_empty_page;

  void setEvidenceImage(QString filename);

  void setupDebouncer();

private slots:
  void debounceFired();
  void on_list_itemChanged(QListWidgetItem *item);
  void on_list_itemActivated(QListWidgetItem *item);
  void on_name_textChanged();
  void on_description_textChanged();
  void on_image_linkActivated();
  void on_delete_button_clicked();

public slots:
  void setEvidenceList(QVector<evi_type> list);
  void togglePresenting(bool presenting);
};

#endif // AOEVIDENCE_H
