#include <QCheckBox>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QLineEdit>
#include <QMenu>
#include <QStringBuilder>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUiLoader>
#include <QVBoxLayout>

#include "aoapplication.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "flowlayout.hpp"
#include "include/widgets/aocharselect.hpp"
#include "options.h"
#include "widgets/aocharbutton.hpp"

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

  QLayout *l_layout = new QVBoxLayout{this};
  ui_main_widget = l_loader.load(&l_uiFile, this);
  l_layout->addWidget(ui_main_widget);

  FROM_UI(QTreeWidget, char_name_tree)
  ui_char_name_tree->setHeaderHidden(true);
  ui_char_name_tree->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui_char_name_tree, &QTreeWidget::itemDoubleClicked, this,
          &AOCharSelect::onCharacterListDoubleClicked);
  connect(ui_char_name_tree, &QTreeWidget::customContextMenuRequested, this,
          &AOCharSelect::onCharacterItemContextMenuRequested);

  FROM_UI(QPushButton, back_to_lobby_button)
  connect(ui_back_to_lobby_button, &QPushButton::pressed, this,
          &AOCharSelect::returnToLobbyPressed);

  FROM_UI(QLineEdit, char_name_edit)
  connect(ui_char_name_edit, &QLineEdit::textChanged, this,
          &AOCharSelect::onSearchChanged);

  FROM_UI(QCheckBox, show_taken_char_button)
  connect(ui_show_taken_char_button, &QCheckBox::clicked, this,
          &AOCharSelect::onShowTakenChanged);

  FROM_UI(QWidget, button_area_widget)
  FROM_UI(QPushButton, spectator_button)
  connect(ui_spectator_button, &QPushButton::pressed,
          this, [this]{emit characterSelected(SPECTATOR_INDEX);});

  ui_flow_layout = new FlowLayout(ui_button_area_widget);

  buildCharacterList(f_characters);
}

void AOCharSelect::setTakenCharacters(const QVector<bool> &f_taken_state)
{
  for (int index = 0; index < m_character_button_list.size(); index++) {
    AOCharButton *l_current_button = m_character_button_list.at(index);
    l_current_button->setTaken(f_taken_state.at(index));
  }
  filterCharacterList();
}

void AOCharSelect::onSearchChanged(const QString &f_string)
{
  Q_UNUSED(f_string)
  filterCharacterList();
}

void AOCharSelect::onShowTakenChanged(int f_state)
{
  Q_UNUSED(f_state)
  filterCharacterList();
}

void AOCharSelect::onCharacterSelected(const int &f_character_index)
{
  if (f_character_index != -1) {
    QString char_name =
        m_character_button_list.at(f_character_index)->characterName();
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
  bool result = false;
  int l_character_index = p_item->text(1).toInt(&result);
  if (!result) {
    p_item->setExpanded(!p_item->isExpanded());
    return;
  }
  emit characterSelected(l_character_index);
}

void AOCharSelect::onCharacterButtonContextMenuRequested(const QPoint &f_pos)
{
  AOCharButton *l_button = static_cast<AOCharButton *>(sender());
  showCharacterContextMenu(l_button->characterID(), l_button->isTaken(),
                           l_button->characterName(),
                           l_button->mapToGlobal(f_pos));
}

void AOCharSelect::onCharacterItemContextMenuRequested(const QPoint &f_pos)
{
  QTreeWidgetItem *l_item = ui_char_name_tree->itemAt(f_pos);
  if (l_item == nullptr) {
    return;
  }
  bool l_result = false;
  int l_character_index = l_item->text(1).toInt(&l_result);
  if (!l_result) {
    return;
  }
  showCharacterContextMenu(l_character_index, l_item->isDisabled(),
                           l_item->text(0),
                           ui_char_name_tree->viewport()->mapToGlobal(f_pos));
}

void AOCharSelect::buildCharacterList(const QVector<char_type> &f_characters)
{
  if (!m_character_button_list.isEmpty()) {
    foreach (AOCharButton *l_button, m_character_button_list) {
      delete l_button;
    }
    m_character_button_list.clear();
    m_character_item_list.clear();
    ui_char_name_tree->clear();
  }

  for (int l_character_id = 0; l_character_id < f_characters.size();
       l_character_id++) {
    char_type l_character = f_characters.at(l_character_id);

    // button
    AOCharButton *l_button =
        new AOCharButton(ui_button_area_widget, ao_app, l_character_id,
                         l_character.taken, l_character.name);
    ui_flow_layout->addWidget(l_button);
    l_button->setContextMenuPolicy(Qt::CustomContextMenu);
    m_character_button_list.append(l_button);

    connect(l_button, &AOCharButton::characterSelected, this,
            &AOCharSelect::onCharacterSelected);
    connect(l_button, &AOCharButton::customContextMenuRequested, this,
            &AOCharSelect::onCharacterButtonContextMenuRequested);

    // tree widget item
    QTreeWidgetItem *l_character_item = new QTreeWidgetItem;
    m_character_item_list.append(l_character_item);
    l_character_item->setText(0, l_character.name);
    l_character_item->setIcon(
        0, QIcon(ao_app->get_image_suffix(
               ao_app->get_character_path(l_character.name, "char_icon"))));
    l_character_item->setText(1, QString::number(l_character_id));

    QString l_category = ao_app->get_category(l_character.name);
    if (!l_category.isEmpty()) {
      QTreeWidgetItem *l_category_item = nullptr;
      QList<QTreeWidgetItem *> l_category_list =
          ui_char_name_tree->findItems(l_category, Qt::MatchFixedString);
      if (!l_category_list.isEmpty()) {
        l_category_item = l_category_list.first();
      }
      else {
        l_category_item = new QTreeWidgetItem;
        l_category_item->setText(0, l_category);
        l_category_item->setChildIndicatorPolicy(
            QTreeWidgetItem::DontShowIndicatorWhenChildless);
        ui_char_name_tree->addTopLevelItem(l_category_item);
      }
      l_category_item->addChild(l_character_item);
    }
    else {
      ui_char_name_tree->addTopLevelItem(l_character_item);
    }
  }
  ui_char_name_tree->expandAll();
  filterCharacterList();
}

void AOCharSelect::filterCharacterList()
{
  QString l_filter = ui_char_name_edit->text();
  bool l_hide_taken_characters = !ui_show_taken_char_button->isChecked();
  for (int i = 0; i < m_character_button_list.size(); ++i) {
    AOCharButton *l_character_button = m_character_button_list.at(i);
    QTreeWidgetItem *l_character_item = m_character_item_list.at(i);
    bool l_is_character_taken = l_character_button->isTaken();
    l_character_button->setDisabled(l_is_character_taken);
    l_character_item->setDisabled(l_is_character_taken);

    bool l_hide_character = false;
    if (!l_filter.isEmpty() && !l_character_button->characterName().contains(
                                   l_filter, Qt::CaseInsensitive)) {
      l_hide_character = true;
    }
    if (l_hide_taken_characters && l_is_character_taken) {
      l_hide_character = true;
    }
    l_character_button->setHidden(l_hide_character);
    l_character_item->setHidden(l_hide_character);
  }
}

void AOCharSelect::showCharacterContextMenu(int f_character_index, bool f_taken,
                                            const QString &f_character,
                                            const QPoint &f_pos)
{

  QString l_ini_path = ao_app->get_real_path(
      ao_app->get_character_path(f_character, "char.ini"));
  if (!QFile::exists(l_ini_path)) {
    call_error(
        tr("Could not find character (char.ini) for %1").arg(f_character));
    return;
  }
  QMenu *l_menu = new QMenu{this};
  l_menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction *l_select_character =
      l_menu->addAction("Select " % f_character, this,
                        [=] { emit characterSelected(f_character_index); });
  l_select_character->setDisabled(f_taken);
  l_menu->addAction(QString("Edit " + f_character + "/char.ini"), this, [=] {
    QDesktopServices::openUrl(QUrl::fromLocalFile(l_ini_path));
  });
  l_menu->addSeparator();
  l_menu->addAction(QString("Open character folder " + f_character), this, [=] {
    QString p_path =
        ao_app->get_real_path(VPath("characters/" + f_character + "/"));
    if (!dir_exists(p_path)) {
      return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
  });
  l_menu->popup(f_pos);
}
