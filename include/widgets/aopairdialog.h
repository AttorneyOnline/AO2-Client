#ifndef AOPAIRDIALOG_H
#define AOPAIRDIALOG_H

#include <QDialog>

class QAbstractButton;
class QDialogButtonBox;
class QListWidget;
class QSlider;

struct char_type;

class AOPairDialog : public QDialog
{
  Q_OBJECT
public:
  explicit AOPairDialog(QWidget *parent);

private:
  QListWidget *ui_char_list;
  QSlider *ui_offset_slider;
  QDialogButtonBox *ui_buttons;

signals:
  void pairChanged(int cid);
  void offsetChanged(int offset);

public slots:
  void setCharacters(const QVector<char_type> &characters);
  void setCurrentPair(int cid);
  void setPairOffset(int offset);

private slots:
  void on_buttons_clicked(QAbstractButton *button);

};


#endif // AOPAIRDIALOG_H
