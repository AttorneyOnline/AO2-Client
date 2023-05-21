#ifndef CHARBUTTON_H
#define CHARBUTTON_H

#include <QPushButton>

namespace AttorneyOnline {
  namespace UI {
    class CharButton : public QPushButton {
    public:
      explicit CharButton(QWidget *parent) : QPushButton(parent){}
      ~CharButton() = default;

      virtual void setTaken(const bool &f_state) = 0;
      virtual bool isTaken() = 0;
      virtual int characterID() = 0;
      virtual QString characterName() = 0;
    };
  } // namespace UI
} // namespace AttorneyOnline

#endif // CHARBUTTON_H
