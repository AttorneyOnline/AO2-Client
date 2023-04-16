#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include "interfaces/widgets/charbutton.hpp"

class AOApplication;

class AOCharButton : public AttorneyOnline::UI::CharButton {
  Q_OBJECT
public:
  explicit AOCharButton(QWidget *parent, AOApplication *f_ao_app,
                        const int &f_id, const bool &f_taken,
                        const QString &f_name);
  ~AOCharButton();

  void setTaken(const bool &f_state) override;
  bool isTaken() override;
  int characterID() override;
  QString characterName() override;

  using AttorneyOnline::UI::CharButton::sizeHint;
  QSize sizeHint();

private:
  AOApplication *ao_app;
  QWidget *ui_taken_overlay;
  QWidget *ui_selector_overlay;

  int m_character_id;
  bool m_taken;
  QString m_character_name;

  const int fixed_height = 60;
  const int fixed_width = 60;

signals:
  void characterSelected(int f_character_id);

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *e) override;
#else
  void enterEvent(QEnterEvent *e) override;
#endif
  void leaveEvent(QEvent *e) override;
};

#endif // AOCHARBUTTON_H
