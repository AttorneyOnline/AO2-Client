#include <QCheckBox>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QLineEdit>
#include <QMenu>
#include <QOpenGLWidget>
#include <QTreeWidget>
#include <QUiLoader>

#include "aoapplication.h"
#include "widgets/aocharbutton.hpp"
#include "debug_functions.h"
#include "file_functions.h"
#include "include/widgets/aocharselect.hpp"
#include "options.h"
#include "flowlayout.hpp"

#define FROM_UI(type, name)                                                    \
  ;                                                                            \
  ui_##name = findChild<type *>(#name);

AOCharSelect::AOCharSelect(QWidget *parent, AOApplication *p_ao_app)
    : AttorneyOnline::UI::Charselect(parent), ao_app(p_ao_app)
{
  setAttribute(Qt::WA_DeleteOnClose);
}

AOCharSelect::~AOCharSelect() {}

void AOCharSelect::loadUI(const QVector<char_type> &f_characters)
{
  QUiLoader l_loader(this);
  QFile l_uiFile(Options::getInstance().getUIAsset(DEFAULT_UI));
  if (!l_uiFile.open(QFile::ReadOnly)) {
    qCritical() << "Unable to open file " << l_uiFile.fileName();
    return;
  }

  ui_main_widget = l_loader.load(&l_uiFile, this);
  FROM_UI(QTreeWidget, char_name_tree)
  FROM_UI(QPushButton, back_to_lobby_button)
  FROM_UI(QLineEdit, char_name_edit)
  FROM_UI(QCheckBox, show_taken_char_button)
  FROM_UI(QWidget, button_area_widget)
  FROM_UI(QPushButton, spectator_button)

  ui_flow_layout = new FlowLayout(ui_button_area_widget);

  buildCharacterList(f_characters);
}

void AOCharSelect::setTakenCharacters(const QVector<bool> &f_taken_state)
{
  for (int index = 0; index < m_char_button_list.size(); index++) {
    AOCharButton *l_current_button = m_char_button_list.at(index);
    l_current_button->setTaken(f_taken_state.at(index));
    if (f_taken_state.at(index)) {
      l_current_button->setDisabled(true);
    }
    else {
      l_current_button->setDisabled(false);
    }
  }
}

void AOCharSelect::onSearchChanged(const QString &f_string)
{
  Q_UNUSED(f_string)
  filterCharacterList();
}

void AOCharSelect::onShowTakenChanged(const bool &f_state)
{
  Q_UNUSED(f_state)
  filterCharacterList();
}

void AOCharSelect::onCharacterSelected(const int &f_character_index)
{
  if (f_character_index != -1) {
    QString char_name = m_char_button_list.at(f_character_index)->characterName();
    QString char_ini_path = ao_app->get_real_path(
        ao_app->get_character_path(char_name, "char.ini"));

    if (!file_exists(char_ini_path)) {
      call_error(
          tr("Could not find character (char.ini) for %1").arg(char_name));
      return;
    }

    qDebug() << "Found char.ini for" << char_name << "at" << char_ini_path;
    emit characterSelected(f_character_index);
    return;
  }
  emit characterSelected(SPECTATOR_INDEX);
}

void AOCharSelect::onCharacterListDoubleClicked(QTreeWidgetItem *p_item,
                                                int column)
{
  Q_UNUSED(column);
  int cid = p_item->text(1).toInt();
  if (cid == -1 && !p_item->isExpanded()) {
    p_item->setExpanded(true);
    return;
  }
  else if (cid == -1) {
    p_item->setExpanded(false);
    return;
  }
  emit characterSelected(cid);
}

void AOCharSelect::onCharacterContextMenuRequested(const QPoint &f_position)
{
}

void AOCharSelect::filterCharacterList()
{

}

void AOCharSelect::buildCharacterList(const QVector<char_type> &f_characters)
{
  if (m_char_button_list.size() > 0) {
    foreach (AOCharButton *l_button, m_char_button_list) {
      delete l_button;
    }
    m_char_button_list.clear();
    ui_char_name_tree->clear();
  }

  // Idk what level of dark magic is performed here.
  for (int l_char_id = 0; l_char_id < f_characters.size(); l_char_id++) {
    char_type l_character = f_characters.at(l_char_id);
    AOCharButton* l_button = new AOCharButton(nullptr, ao_app, l_char_id, l_character.taken, l_character.name);
    ui_flow_layout->addWidget(l_button);
    l_button->setContextMenuPolicy(Qt::CustomContextMenu);
    m_char_button_list.append(l_button);

    connect(l_button, &AOCharButton::characterSelected, this,
            &AOCharSelect::onCharacterSelected);
  }
}
