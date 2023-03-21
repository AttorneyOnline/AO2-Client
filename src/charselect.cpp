#include "courtroom.h"
#include "lobby.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"

void Courtroom::construct_char_select()
{
  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ui_char_select_background = new AOImage(this, ao_app);
  ui_char_select_background->setObjectName("ui_char_select_background");

  ui_char_list = new QTreeWidget(ui_char_select_background);
  ui_char_list->setColumnCount(2);
  ui_char_list->setHeaderLabels({"Name", "ID"});
  ui_char_list->setHeaderHidden(true);
  ui_char_list->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui_char_list->hideColumn(1);
  ui_char_list->setDropIndicatorShown(true);
  ui_char_list->setObjectName("ui_char_list");

  ui_char_buttons = new QWidget(ui_char_select_background);
  ui_char_buttons->setObjectName("ui_char_buttons");

  ui_back_to_lobby = new AOButton(ui_char_select_background, ao_app);
  ui_back_to_lobby->setObjectName("ui_back_to_lobby");

  ui_char_password = new QLineEdit(ui_char_select_background);
  ui_char_password->setPlaceholderText(tr("Password"));
  ui_char_password->setObjectName("ui_char_password");

  ui_char_select_left = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_left->setObjectName("ui_char_select_left");
  ui_char_select_right = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_right->setObjectName("ui_char_select_right");

  ui_spectator = new AOButton(ui_char_select_background, ao_app);
  ui_spectator->setText(tr("Spectator"));
  ui_spectator->setObjectName("ui_spectator");

  ui_char_search = new QLineEdit(ui_char_select_background);
  ui_char_search->setPlaceholderText(tr("Search"));
  ui_char_search->setObjectName("ui_char_search");

  ui_char_passworded = new QCheckBox(ui_char_select_background);
  ui_char_passworded->setText(tr("Passworded"));
  ui_char_passworded->setObjectName("ui_char_passworded");

  ui_char_taken = new QCheckBox(ui_char_select_background);
  ui_char_taken->setText(tr("Taken"));
  ui_char_taken->setObjectName("ui_char_taken");

  connect(ui_char_list, &QTreeWidget::itemDoubleClicked,
          this, &Courtroom::on_char_list_double_clicked);

  connect(ui_back_to_lobby, &AOButton::clicked, this,
          &Courtroom::on_back_to_lobby_clicked);

  connect(ui_char_select_left, &AOButton::clicked, this,
          &Courtroom::on_char_select_left_clicked);
  connect(ui_char_select_right, &AOButton::clicked, this,
          &Courtroom::on_char_select_right_clicked);

  connect(ui_spectator, &AOButton::clicked, this, &Courtroom::on_spectator_clicked);

  connect(ui_char_search, &QLineEdit::textEdited,this,
          &Courtroom::on_char_search_changed);
  connect(ui_char_passworded, &QCheckBox::stateChanged, this,
          &Courtroom::on_char_passworded_clicked);
  connect(ui_char_taken, &QCheckBox::stateChanged, this,
           &Courtroom::on_char_taken_clicked);

}

void Courtroom::set_char_select()
{
  QString filename = "courtroom_design.ini";

  pos_size_type f_charselect =
      ao_app->get_element_dimensions("char_select", filename);

  if (f_charselect.width < 0 || f_charselect.height < 0) {
    qWarning() << "did not find char_select width or height in "
                "courtroom_design.ini!";
    this->setFixedSize(714, 668);
  }
  else
    this->setFixedSize(f_charselect.width, f_charselect.height);
  ui_char_select_background->resize(f_charselect.width, f_charselect.height);
  ui_char_select_background->set_image("charselect_background");

  ui_char_search->setFocus();
  set_size_and_pos(ui_char_search, "char_search");
  set_size_and_pos(ui_char_list, "char_list");
  set_size_and_pos(ui_char_passworded, "char_passworded");
  set_size_and_pos(ui_char_taken, "char_taken");
  set_size_and_pos(ui_char_buttons, "char_buttons");

  // Silence emission. This causes the signal to be emitted TWICE during server join!
  // Fuck this. Performance Sandwich.
  ui_char_taken->blockSignals(true);
  ui_char_passworded->blockSignals(true);
  ui_char_taken->setChecked(true);
  ui_char_passworded->setChecked(true);
  ui_char_taken->blockSignals(false);
  ui_char_passworded->blockSignals(false);

  truncate_label_text(ui_char_taken, "char_taken");
  truncate_label_text(ui_char_passworded, "char_passworded");

  filter_character_list();

  ui_char_search->setFocus();
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_char_select_left->hide();
  ui_char_select_right->hide();

  for (AOCharButton *i_button : qAsConst(ui_char_button_list)) {
    i_button->reset();
    i_button->hide();
    i_button->move(0, 0);
  }

  int total_pages = ui_char_button_list_filtered.size() / max_chars_on_page;
  int chars_on_page = 0;

  if (ui_char_button_list_filtered.size() % max_chars_on_page != 0) {
    ++total_pages;
    // i. e. not on the last page
    if (total_pages > current_char_page + 1)
      chars_on_page = max_chars_on_page;
    else
      chars_on_page = ui_char_button_list_filtered.size() % max_chars_on_page;
  }
  else
    chars_on_page = max_chars_on_page;

  if (total_pages > current_char_page + 1)
    ui_char_select_right->show();

  if (current_char_page > 0)
    ui_char_select_left->show();

  QPoint f_spacing =
      ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  char_columns =
      ((ui_char_buttons->width() - button_width) / (f_spacing.x() + button_width)) +
      1;
  char_rows = ((ui_char_buttons->height() - button_height) /
               (f_spacing.y() + button_height)) +
              1;

  max_chars_on_page = char_columns * char_rows;

  put_button_in_place(current_char_page * max_chars_on_page, chars_on_page);
}

void Courtroom::on_char_list_double_clicked(QTreeWidgetItem *p_item, int column)
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
  char_clicked(cid);
}

void Courtroom::char_clicked(int n_char)
{
  if (n_char != -1)
  {
    QString char_name = char_list.at(n_char).name;
    QString char_ini_path = ao_app->get_real_path(
          ao_app->get_character_path(char_name, "char.ini"));

    if (!file_exists(char_ini_path)) {
      call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
      return;
    }

    qDebug() << "Found char.ini for" << char_name << "at" << char_ini_path;
  }

  if (n_char != m_cid || n_char == -1) {
    ao_app->send_server_packet(
        new AOPacket("PW", {ui_char_password->text()}));
    ao_app->send_server_packet(
        new AOPacket("CC", {QString::number(ao_app->client_id),
                            QString::number(n_char), get_hdid()}));
  }
  if (n_char == m_cid || n_char == -1) {
    update_character(n_char);
    enter_courtroom();
    set_courtroom_size();
  }
}

void Courtroom::on_char_button_context_menu_requested(const QPoint &pos)
{
  AOCharButton* button = qobject_cast<AOCharButton*>(sender());
  int n_char = ui_char_button_list.indexOf(button);
  if (n_char == -1)
  {
    return;
  }

  QString char_name = char_list.at(n_char).name;
  QString char_ini_path = ao_app->get_real_path(
        ao_app->get_character_path(char_name, "char.ini"));

  if (!file_exists(char_ini_path)) {
    call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
    return;
  }

  QMenu *menu = new QMenu(this);
  menu->addAction(QString("Edit " + char_name + "/char.ini"), this,
                  [=] { QDesktopServices::openUrl(QUrl::fromLocalFile(char_ini_path)); }
  );
  menu->addSeparator();
  menu->addAction(QString("Open character folder " + char_name), this,
                  [=] {
    QString p_path = ao_app->get_real_path(VPath("characters/" + char_name + "/"));
    if (!dir_exists(p_path)) {
      return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
  }
  );
  menu->popup(button->mapToGlobal(pos));
}

void Courtroom::put_button_in_place(int starting, int chars_on_this_page)
{
  if (ui_char_button_list_filtered.size() == 0)
    return;

  QPoint f_spacing =
      ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  int x_mod_count = 0;
  int y_mod_count = 0;

  int startout = starting;
  for (int n = starting; n < startout + chars_on_this_page; ++n) {
    int x_pos = (button_width + f_spacing.x()) * x_mod_count;
    int y_pos = (button_height + f_spacing.y()) * y_mod_count;

    ui_char_button_list_filtered.at(n)->move(x_pos, y_pos);
    ui_char_button_list_filtered.at(n)->show();
    ui_char_button_list_filtered.at(n)->apply_taken_image();

    ++x_mod_count;

    if (x_mod_count == char_columns) {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}

void Courtroom::character_loading_finished()
{
  // Zeroeth, we'll clear any leftover characters from previous server visits.
  ao_app->generated_chars = 0;
  if (ui_char_button_list.size() > 0) {
    foreach (AOCharButton *item, ui_char_button_list) {
      delete item;
    }
    ui_char_button_list.clear();
    ui_char_list->clear();
  }

  // First, we'll make all the character buttons in the very beginning.
  // We also hide them all, so they can't be accidentally clicked.
  // Later on, we'll be revealing buttons as we need them.
  for (int n = 0; n < char_list.size(); n++) {
    AOCharButton *char_button =
        new AOCharButton(ui_char_buttons, ao_app, 0, 0, char_list.at(n).taken);
    char_button->setContextMenuPolicy(Qt::CustomContextMenu);
    char_button->reset();
    char_button->hide();
    char_button->set_image(char_list.at(n).name);
    char_button->setToolTip(char_list.at(n).name);
    ui_char_button_list.append(char_button);
    QString char_category = ao_app->get_category(char_list.at(n).name);
    QList<QTreeWidgetItem*> matching_list = ui_char_list->findItems(char_category, Qt::MatchFixedString, 0);
    // create the character tree item
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, char_list.at(n).name);
    treeItem->setIcon(0, QIcon(ao_app->get_image_suffix(
      ao_app->get_character_path(char_list.at(n).name, "char_icon"))));
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
      category->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
      ui_char_list->insertTopLevelItem(0, category);
      category->addChild(treeItem);
    }
      
    
    connect(char_button, &AOCharButton::clicked,
            [this, n]() { this->char_clicked(n); });
    connect(char_button, &AOCharButton::customContextMenuRequested, this, &Courtroom::on_char_button_context_menu_requested);

    // This part here serves as a way of showing to the player that the game is
    // still running, it is just loading the pictures of the characters.
    if (ao_app->lobby_constructed) {
      ao_app->generated_chars++;
    }
  }
  ui_char_list->expandAll();
}

void Courtroom::filter_character_list()
{
  ui_char_button_list_filtered.clear();
  for (int i = 0; i < char_list.size(); i++) {
    AOCharButton *current_char = ui_char_button_list.at(i);
    QTreeWidgetItem* current_char_list_item = ui_char_list->findItems(QString::number(i), Qt::MatchExactly | Qt::MatchRecursive, 1)[0];



    // It seems passwording characters is unimplemented yet?
    // Until then, this will stay here, I suppose.
    // if (ui_char_passworded->isChecked() && character_is_passworded??)
    //    continue;

    if (!ui_char_taken->isChecked() && char_list.at(i).taken) {
      current_char_list_item->setHidden(true);
      continue;
    }

    if (!char_list.at(i).name.contains(ui_char_search->text(),
                                       Qt::CaseInsensitive)) {
      current_char_list_item->setHidden(true);
      continue;
  }

    // We only really need to update the fact that a character is taken
    // for the buttons that actually appear.
    // You'd also update the passwordedness and etc. here later.
    current_char->reset();
    current_char_list_item->setHidden(false);
    current_char->set_taken(char_list.at(i).taken);
    current_char_list_item->setText(0, char_list.at(i).name);
    // reset disabled
    current_char_list_item->setDisabled(false);
    if (char_list.at(i).taken) // woops, we are taken
      current_char_list_item->setDisabled(true);

    ui_char_button_list_filtered.append(current_char);
  }

  current_char_page = 0;
  set_char_select_page();
}

void Courtroom::on_char_search_changed() { filter_character_list(); }

void Courtroom::on_char_passworded_clicked() { filter_character_list(); }

void Courtroom::on_char_taken_clicked() { filter_character_list(); }
