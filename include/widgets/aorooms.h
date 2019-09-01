#ifndef AOROOMS_H
#define AOROOMS_H

#include "aoapplication.h"

#include <QLineEdit>
#include <QListWidget>
#include <QWidget>

class AORoomChooser : public QWidget
{
  Q_OBJECT
public:
  explicit AORoomChooser(QWidget *parent, AOApplication *p_ao_app);

  void setAreas(QVector<area_type> &areas);
private:
  AOApplication *ao_app;

  QListWidget *ui_area_list;
  QLineEdit *ui_search;

  QVector<area_type> area_list;

signals:
  void roomSelected(QString room);

public slots:
  void refresh();

private slots:
  void on_area_list_doubleClicked(QModelIndex index);
  void on_search_textEdited(QString);
};

#endif // AOROOMS_H
