#ifndef AOCHARSELECT_H
#define AOCHARSELECT_H
#pragma once

#include "interfaces/widgets/charselect.hpp"

class QTreeWidget;
class QLineEdit;
class QCheckBox;
class AOApplication;
class AOCharButton;

class AOCharSelect : public AttorneyOnline::UI::Charselect {
  Q_OBJECT

public:
  explicit AOCharSelect(QWidget *parent, AOApplication *p_ao_app);
  ~AOCharSelect();

  void loadUI(const QVector<char_type> &f_characters) override;
  void setTakenCharacters(const QVector<bool> &f_taken_state) override;

private slots:
  void onSearchChanged(const QString &f_string) override;
  void onShowTakenChanged(const bool &f_state) override;
  void onCharacterSelected(const int &f_character_index) override;
  void onCharacterListDoubleClicked(QTreeWidgetItem *p_item, int column) override;
  void onCharacterContextMenuRequested(const QPoint &pos) override;

signals:
  void characterSelected(const int &f_character_index);

private:
  void filterCharacterList();
  void buildCharacterList(const QVector<char_type> &f_characters) override;

  const QString DEFAULT_UI = "lobby.ui";
  const int SPECTATOR_INDEX = -1;

  QWidget *main_widget;
  QTreeWidget *ui_char_list;
  QLineEdit *ui_name_edit;
  QCheckBox *ui_taken_box;

  QVector<char_type> m_character_list;
  QVector<AOCharButton *> m_char_button_list;
  AOApplication *ao_app;
};

#endif // AOCHARSELECT_H
