#ifndef CHARSELECT_H
#define CHARSELECT_H
#pragma once

#include <QWidget>

namespace AttorneyOnline {
  namespace UI {
    class Charselect : public QWidget {
    public:
      Charselect(QObject *parent);
      ~Charselect() = default;

      virtual void loadUI() = 0;
      virtual void buildCharacterList(const QStringList &f_characters) = 0;
      virtual void createCharacterButton(QString f_character) = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline

#endif // CHARSELECT_H
