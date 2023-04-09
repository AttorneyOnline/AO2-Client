#ifndef CHARSELECT_H
#define CHARSELECT_H
#pragma once

#include "datatypes.h"
#include <QDialog>

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
      virtual void
      buildCharacterList(const QVector<char_type> &f_characters) = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline

#endif // CHARSELECT_H
