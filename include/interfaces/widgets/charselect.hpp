#ifndef CHARSELECT_H
#define CHARSELECT_H
#pragma once

#include <QDialog>
#include "datatypes.h"

class QTreeWidgetItem;
namespace AttorneyOnline {
  namespace UI {
    class Charselect : public QWidget {
      Q_OBJECT

    public:
      Charselect(QWidget *parent) : QWidget(parent) {}
      ~Charselect() = default;

      virtual void loadUI(const QVector<char_type> &f_characters) = 0;
      virtual void setTakenCharacters(const QVector<bool> &f_taken_state) = 0;
      virtual void buildCharacterList(const QVector<char_type> &f_characters) = 0;

    public slots:
      virtual void onSearchChanged(const QString &f_string) = 0;
      virtual void onShowTakenChanged(const bool &f_state) = 0;
      virtual void onCharacterSelected(const int &f_character_index) = 0;
      virtual void onCharacterListDoubleClicked(QTreeWidgetItem *p_item, int column) = 0;
      virtual void onCharacterContextMenuRequested(const QPoint &pos) = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline

#endif // CHARSELECT_H
