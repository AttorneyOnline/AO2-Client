#include "courtroom.h"
#include "lobby.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"

#include <QElapsedTimer>
#include <QHash>
#include <QMessageBox>
#include <QPixmapCache>
#include <QTimer>

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

  char_icon_warmup_timer = new QTimer(this);
  // Let the UI breathe between slices; 0 ms would keep the event loop busy.
  char_icon_warmup_timer->setInterval(100);
  connect(char_icon_warmup_timer, &QTimer::timeout, this,
          &Courtroom::warm_char_icons_step);
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
  start_char_icon_warmup();

  ui_char_search->setFocus();
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_char_select_left->hide();
  ui_char_select_right->hide();
  QPoint f_spacing =
      ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  int s_button_size = static_cast<int>(button_width * Options::getInstance().themeScalingFactor());

  char_columns =
      ((ui_char_buttons->width() - s_button_size) / (f_spacing.x() + s_button_size)) +
      1;
  char_rows = ((ui_char_buttons->height() - s_button_size) /
               (f_spacing.y() + s_button_size)) +
              1;
  max_chars_on_page = qMax(1, char_columns * char_rows);

  for (AOCharButton *i_button : std::as_const(ui_char_button_list)) {
    i_button->reset();
    i_button->hide();
    i_button->move(0, 0);
  }

  const int filtered_size = ui_char_button_list_filtered.size();
  if (filtered_size == 0)
    return;

  const int total_pages = (filtered_size + max_chars_on_page - 1) / max_chars_on_page;
  current_char_page = qBound(0, current_char_page, total_pages - 1);

  if (total_pages > current_char_page + 1)
    ui_char_select_right->show();

  if (current_char_page > 0)
    ui_char_select_left->show();

  ensure_char_button_pool(max_chars_on_page);

  const int starting = current_char_page * max_chars_on_page;
  const int chars_on_page = qMin(max_chars_on_page, filtered_size - starting);

  // Make sure the visible page is resolved before the buttons ask for their
  // icons. Usually this is a pure cache hit thanks to the idle warmup.
  for (int i = 0; i < chars_on_page; ++i)
    resolve_char_icon(ui_char_button_list_filtered.at(starting + i));

  put_button_in_place(starting, chars_on_page);
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
    ao_app->net_manager->streamed_charname = char_name;

    if (!file_exists(char_ini_path)) {
      if (ao_app->asset_url.isEmpty()) {
        call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
        return;
      } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Could not find character (char.ini) for %1.\nAvailable asset url: %2").arg(char_name).arg(ao_app->asset_url));
        msgBox.setInformativeText(tr("Do you want to download the character or use it via streaming?"));
        QPushButton* btn1 = msgBox.addButton(tr("Download"), QMessageBox::AcceptRole);
        QPushButton* btn2 = msgBox.addButton(tr("Streaming"), QMessageBox::AcceptRole);
        QPushButton* btn3 = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(btn3);
        msgBox.exec();
    
        if (msgBox.clickedButton() == btn1) {
            // I WILL change this later...
            QString download_path = VPath(ao_app->asset_url + "characters/" + char_name + "/").toQString();
            QString download_path_lower = VPath(ao_app->asset_url + "characters/" + char_name.toLower() + "/").toQString();
            QStringList paths_to_download;
            paths_to_download << download_path_lower << download_path;
            qDebug() << paths_to_download;
            ao_app->net_manager->download_folder(paths_to_download);
            return;
        } else if (msgBox.clickedButton() == btn2) {
            QString streamed_ini_path = VPath(ao_app->asset_url + "characters/" + char_name + "/char.ini").toQString();
            QString streamed_ini_path_lower = VPath(ao_app->asset_url + "characters/" + char_name.toLower() + "/char.ini").toQString();
            qDebug() << streamed_ini_path;
            ao_app->net_manager->start_image_streaming(streamed_ini_path, "");
            ao_app->net_manager->start_image_streaming(streamed_ini_path_lower, "");
        } else {
            return;
        }
        return;
      }
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
  if (button == nullptr)
  {
    return;
  }
  int n_char = button->char_id;
  if (n_char < 0 || n_char >= char_list.size())
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

void Courtroom::ensure_char_button_pool(int count)
{
  while (ui_char_button_list.size() < count) {
    AOCharButton *char_button =
        new AOCharButton(ui_char_buttons, ao_app, 0, 0, false);
    char_button->setContextMenuPolicy(Qt::CustomContextMenu);
    char_button->reset();
    char_button->hide();
    ui_char_button_list.append(char_button);

    connect(char_button, &AOCharButton::clicked, this, [this, char_button]() {
      if (char_button->char_id >= 0)
        this->char_clicked(char_button->char_id);
    });
    connect(char_button, &AOCharButton::customContextMenuRequested, this,
            &Courtroom::on_char_button_context_menu_requested);
  }
}

void Courtroom::put_button_in_place(int starting, int chars_on_this_page)
{
  if (ui_char_button_list_filtered.size() == 0)
    return;

  QPoint f_spacing =
      ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  int x_mod_count = 0;
  int y_mod_count = 0;

  int size = static_cast<int>(button_width * Options::getInstance().themeScalingFactor());
  for (int i = 0; i < chars_on_this_page; ++i) {
    const int char_id = ui_char_button_list_filtered.at(starting + i);
    const char_type &f_char = char_list.at(char_id);
    AOCharButton *char_button = ui_char_button_list.at(i);

    int x_pos = (size + f_spacing.x()) * x_mod_count;
    int y_pos = (size + f_spacing.y()) * y_mod_count;

    if (char_button->char_id != char_id) {
      char_button->char_id = char_id;
      char_button->set_image(f_char.name);
      char_button->setToolTip(f_char.name);
    }
    char_button->reset();
    char_button->set_taken(f_char.taken);
    char_button->move(x_pos, y_pos);
    char_button->show();
    char_button->apply_taken_image();

    ++x_mod_count;

    if (x_mod_count == char_columns) {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}

void Courtroom::character_loading_finished()
{
  m_cid = -1;

  const QObjectList char_button_children = ui_char_buttons->children();
  for (QObject *child : char_button_children)
    delete child;
  ui_char_button_list.clear();
  ui_char_button_list_filtered.clear();

  ui_char_list->clear();
  ui_char_list_items.clear();
  ui_char_list_items.reserve(char_list.size());

  QHash<QString, QTreeWidgetItem *> categories;

  for (int n = 0; n < char_list.size(); n++) {
    const char_type &f_char = char_list.at(n);
    const QString char_category = f_char.category;

    // create the character tree item
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, f_char.name);
    treeItem->setText(1, QString::number(n));
    ui_char_list_items.append(treeItem);

    // category logic
    if (char_category == "") { // no category
      ui_char_list->addTopLevelItem(treeItem);
    }
    else {
      const QString category_key = char_category.toLower();
      QTreeWidgetItem *category = categories.value(category_key, nullptr);
      if (category == nullptr) { // we need to make a new category
        category = new QTreeWidgetItem();
        category->setText(0, char_category);
        category->setText(1, "-1");
        category->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
        ui_char_list->insertTopLevelItem(0, category);
        categories.insert(category_key, category);
      }
      category->addChild(treeItem);
    }
  }
  ui_char_list->expandAll();

  stop_char_icon_warmup();
  char_icon_warmup_index = 0;

  if (ui_char_select_background->isVisible()) {
    filter_character_list();
    start_char_icon_warmup();
  }
}

void Courtroom::resolve_char_icon(int char_id)
{
  if (char_id < 0 || char_id >= char_list.size())
    return;

  // The result is intentionally discarded: get_real_path() caches both hits
  // and misses, so this is what makes later per-button lookups free.
  ao_app->get_real_path(
      ao_app->get_character_path(char_list.at(char_id).name, "char_icon"),
      {".png"});
}

void Courtroom::start_char_icon_warmup()
{
  if (char_icon_warmup_timer == nullptr || char_list.isEmpty())
    return;

  if (!char_icon_warmup_timer->isActive()) {
    // Re-walk the list whenever warmup restarts. Cached entries are cheap
    // hash hits, and this also picks up assets added since the last pass.
    char_icon_warmup_index = 0;
    char_icon_warmup_timer->start();
  }
}

void Courtroom::stop_char_icon_warmup()
{
  if (char_icon_warmup_timer != nullptr)
    char_icon_warmup_timer->stop();
}

void Courtroom::warm_char_icons_step()
{
  if (char_icon_warmup_index >= char_list.size()) {
    stop_char_icon_warmup();
    return;
  }

  QElapsedTimer budget;
  budget.start();
  while (char_icon_warmup_index < char_list.size()) {
    resolve_char_icon(char_icon_warmup_index);
    ++char_icon_warmup_index;
    // Keep each slice short. A single cold icon can exceed the budget; that
    // only happens once per asset, and every later page visit is a cache hit.
    if (budget.elapsed() >= 4)
      break;
  }

  if (char_icon_warmup_index >= char_list.size())
    stop_char_icon_warmup();
}

void Courtroom::filter_character_list()
{
  ui_char_button_list_filtered.clear();

  const QString search_text = ui_char_search->text();
  const bool show_taken = ui_char_taken->isChecked();

  for (int i = 0; i < char_list.size(); i++) {
    if (i >= ui_char_list_items.size())
      break;

    QTreeWidgetItem *current_char_list_item = ui_char_list_items.at(i);
    const char_type &f_char = char_list.at(i);

    // It seems passwording characters is unimplemented yet?
    // Until then, this will stay here, I suppose.
    // if (ui_char_passworded->isChecked() && character_is_passworded??)
    //    continue;

    const bool matches_search =
        f_char.name.contains(search_text, Qt::CaseInsensitive) ||
        f_char.category.contains(search_text, Qt::CaseInsensitive);

    if ((!show_taken && f_char.taken) || !matches_search) {
      current_char_list_item->setHidden(true);
      continue;
    }

    current_char_list_item->setHidden(false);
    current_char_list_item->setDisabled(f_char.taken);
    ui_char_button_list_filtered.append(i);
  }

  current_char_page = 0;
  set_char_select_page();
}

void Courtroom::on_char_search_changed() { filter_character_list(); }

void Courtroom::on_char_passworded_clicked() { filter_character_list(); }

void Courtroom::on_char_taken_clicked() { filter_character_list(); }
