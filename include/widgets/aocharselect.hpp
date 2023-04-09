#ifndef AOCHARSELECT_H
#define AOCHARSELECT_H
#pragma once

#include "interfaces/widgets/charselect.hpp"

class QTreeWidget;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QScrollArea;
class FlowLayout;
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

  const QString DEFAULT_UI = "charselect.ui";
  const int SPECTATOR_INDEX = -1;

  QWidget *ui_main_widget;
  QTreeWidget* ui_char_name_tree;
  QPushButton *ui_back_to_lobby_button;
  QLineEdit* ui_char_name_edit;
  QCheckBox* ui_show_taken_char_button;
  QWidget* ui_button_area_widget;
  QPushButton* ui_spectator_button;
  FlowLayout* ui_flow_layout;

  QVector<AOCharButton *> m_char_button_list;
  AOApplication *ao_app;
};

#endif // AOCHARSELECT_H
