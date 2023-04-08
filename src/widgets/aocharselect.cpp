#include "include/widgets/aocharselect.hpp"
#include "aoapplication.h"
#include "aocharbutton.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "options.h"
#include <QCheckBox>
#include <QDesktopServices>
#include <QLineEdit>
#include <QMenu>
#include <QOpenGLWidget>
#include <QTreeWidget>

#include <QDebug>
#include <QFile>
#include <QUiLoader>

AOCharSelect::AOCharSelect(QWidget *parent, AOApplication *p_ao_app)
    : AttorneyOnline::UI::Charselect(parent), ao_app(p_ao_app)
{
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

  l_loader.load(&l_uiFile, this);
}

void AOCharSelect::setTakenCharacters(const QVector<bool> &f_taken_state)
{
  for (int index = 0; index < m_char_button_list.size(); index++) {
    AOCharButton *l_current_button = m_char_button_list.at(index);
    l_current_button->set_taken(f_taken_state.at(index));
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
    QString char_name = m_character_list.at(f_character_index).name;
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
  AOCharButton *button = qobject_cast<AOCharButton *>(sender());
  int n_char = m_char_button_list.indexOf(button);
  if (n_char == -1) {
    return;
  }

  QString char_name = m_character_list.at(n_char).name;
  QString char_ini_path =
      ao_app->get_real_path(ao_app->get_character_path(char_name, "char.ini"));

  if (!file_exists(char_ini_path)) {
    call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
    return;
  }

  QMenu *menu = new QMenu(this);
  menu->addAction(QString("Edit " + char_name + "/char.ini"), this, [=] {
    QDesktopServices::openUrl(QUrl::fromLocalFile(char_ini_path));
  });
  menu->addSeparator();
  menu->addAction(QString("Open character folder " + char_name), this, [=] {
    QString p_path =
        ao_app->get_real_path(VPath("characters/" + char_name + "/"));
    if (!dir_exists(p_path)) {
      return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
  });
  connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
  menu->popup(button->mapToGlobal(f_position));
}

void AOCharSelect::filterCharacterList()
{
  for (int i = 0; i < m_character_list.size(); i++) {
    AOCharButton *current_char = m_char_button_list.at(i);
    QTreeWidgetItem *current_m_character_list_item = ui_char_list->findItems(
        QString::number(i), Qt::MatchExactly | Qt::MatchRecursive, 1)[0];

    // It seems passwording characters is unimplemented yet?
    // Until then, this will stay here, I suppose.
    // if (ui_char_passworded->isChecked() && character_is_passworded??)
    //    continue;

    if (!ui_taken_box->isChecked() && m_character_list.at(i).taken) {
      current_m_character_list_item->setHidden(true);
      continue;
    }

    if (!m_character_list.at(i).name.contains(ui_name_edit->text(),
                                              Qt::CaseInsensitive)) {
      current_m_character_list_item->setHidden(true);
      continue;
    }

    // We only really need to update the fact that a character is taken
    // for the buttons that actually appear.
    // You'd also update the passwordedness and etc. here later.
    current_char->reset();
    current_m_character_list_item->setHidden(false);
    current_char->set_taken(m_character_list.at(i).taken);
    current_m_character_list_item->setText(0, m_character_list.at(i).name);
    // reset disabled
    current_m_character_list_item->setDisabled(false);
    if (m_character_list.at(i).taken) // woops, we are taken
      current_m_character_list_item->setDisabled(true);
  }
}

void AOCharSelect::buildCharacterList(const QVector<char_type> &f_characters)
{
  if (m_char_button_list.size() > 0) {
    foreach (AOCharButton *l_button, m_char_button_list) {
      delete l_button;
    }
    m_char_button_list.clear();
    ui_char_list->clear();
  }

  // First, we'll make all the character buttons in the very beginning.
  // We also hide them all, so they can't be accidentally clicked.
  // Later on, we'll be revealing buttons as we need them.
  for (int n = 0; n < m_character_list.size(); n++) {
    AOCharButton *char_button =
        new AOCharButton(ui_char_buttons, ao_app, 0, 0, m_character_list.at(n).taken);
    char_button->setContextMenuPolicy(Qt::CustomContextMenu);
    char_button->setToolTip(m_character_list.at(n).name);
    m_char_button_list.append(char_button);
    QString char_category = ao_app->get_category(m_character_list.at(n).name);
    QList<QTreeWidgetItem *> matching_list =
        ui_char_list->findItems(char_category, Qt::MatchFixedString, 0);
    // create the character tree item
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, m_character_list.at(n).name);
    treeItem->setIcon(
        0, QIcon(ao_app->get_image_suffix(
               ao_app->get_character_path(m_character_list.at(n).name, "char_icon"))));
    treeItem->setText(1, QString::number(n));
    // category logic
    QTreeWidgetItem *category;
    if (char_category == "") // no category
      ui_char_list->addTopLevelItem(treeItem);
    else if (!matching_list.isEmpty()) { // our category already exists
      category = matching_list[0];
      category->addChild(treeItem);
    }
    else { // we need to make a new category
      category = new QTreeWidgetItem();
      category->setText(0, char_category);
      category->setText(1, "-1");
      category->setChildIndicatorPolicy(
          QTreeWidgetItem::DontShowIndicatorWhenChildless);
      ui_char_list->insertTopLevelItem(0, category);
      category->addChild(treeItem);
    }

    connect(char_button, &AOCharButton::clicked,
            [this, n]() { this->onCharacterSelected(n); });
    connect(char_button, &AOCharButton::customContextMenuRequested, this,
            &AOCharSelect::onCharacterContextMenuRequested);

    // This part here serves as a way of showing to the player that the game is
    // still running, it is just loading the pictures of the characters.
    ui_char_list->expandAll();
  }
}
