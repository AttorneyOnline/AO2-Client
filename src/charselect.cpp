#include "courtroom.h"
#include "lobby.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"

void Courtroom::construct_char_select()
{
  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ui_char_select_background = new AOImage(this, ao_app);

  ui_char_buttons = new QWidget(ui_char_select_background);

  ui_selector = new AOImage(ui_char_select_background, ao_app);
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->resize(62, 62);

  ui_back_to_lobby = new AOButton(ui_char_select_background, ao_app);

  ui_char_password = new QLineEdit(ui_char_select_background);
  ui_char_password->setPlaceholderText(tr("Password"));

  ui_char_select_left = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_right = new AOButton(ui_char_select_background, ao_app);

  ui_spectator = new AOButton(ui_char_select_background, ao_app);
  ui_spectator->setText(tr("Spectator"));

  ui_char_search = new QLineEdit(ui_char_select_background);
  ui_char_search->setPlaceholderText(tr("Search"));
  ui_char_search->setFocus();
  set_size_and_pos(ui_char_search, "char_search");

  ui_char_passworded = new QCheckBox(ui_char_select_background);
  ui_char_passworded->setText(tr("Passworded"));
  set_size_and_pos(ui_char_passworded, "char_passworded");

  ui_char_taken = new QCheckBox(ui_char_select_background);
  ui_char_taken->setText(tr("Taken"));
  set_size_and_pos(ui_char_taken, "char_taken");

  ui_char_taken->setChecked(true);
  ui_char_passworded->setChecked(true);

  set_size_and_pos(ui_char_buttons, "char_buttons");

  connect(ui_back_to_lobby, SIGNAL(clicked()), this,
          SLOT(on_back_to_lobby_clicked()));

  connect(ui_char_select_left, SIGNAL(clicked()), this,
          SLOT(on_char_select_left_clicked()));
  connect(ui_char_select_right, SIGNAL(clicked()), this,
          SLOT(on_char_select_right_clicked()));

  connect(ui_spectator, SIGNAL(clicked()), this, SLOT(on_spectator_clicked()));

  connect(ui_char_search, SIGNAL(textEdited(const QString &)), this,
          SLOT(on_char_search_changed()));
  connect(ui_char_passworded, SIGNAL(stateChanged(int)), this,
          SLOT(on_char_passworded_clicked()));
  connect(ui_char_taken, SIGNAL(stateChanged(int)), this,
          SLOT(on_char_taken_clicked()));
}

void Courtroom::set_char_select()
{
  QString filename = "courtroom_design.ini";

  pos_size_type f_charselect =
      ao_app->get_element_dimensions("char_select", filename);

  if (f_charselect.width < 0 || f_charselect.height < 0) {
    qDebug() << "W: did not find char_select width or height in "
                "courtroom_design.ini!";
    this->setFixedSize(714, 668);
  }
  else
    this->setFixedSize(f_charselect.width, f_charselect.height);

  ui_char_select_background->resize(f_charselect.width, f_charselect.height);
  ui_char_select_background->set_image("charselect_background");

  filter_character_list();

  ui_char_search->setFocus();
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_char_select_left->hide();
  ui_char_select_right->hide();

  for (AOCharButton *i_button : ui_char_button_list) {
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

  put_button_in_place(current_char_page * max_chars_on_page, chars_on_page);
}

void Courtroom::char_clicked(int n_char)
{
  if (n_char != -1)
  {
    QString char_ini_path =
        ao_app->get_character_path(char_list.at(n_char).name, "char.ini");

    qDebug() << "char_ini_path" << char_ini_path;

    if (!file_exists(char_ini_path)) {
      call_notice("Could not find " + char_ini_path);
      return;
    }
  }

  if (n_char != m_cid) {
    ao_app->send_server_packet(
        new AOPacket("PW#" + ui_char_password->text() + "#%"));
    ao_app->send_server_packet(
        new AOPacket("CC#" + QString::number(ao_app->s_pv) + "#" +
                     QString::number(n_char) + "#" + get_hdid() + "#%"));
  }
  else
    update_character(n_char);

  if (n_char != -1)
    ui_ic_chat_name->setPlaceholderText(char_list.at(n_char).name);
}

void Courtroom::put_button_in_place(int starting, int chars_on_this_page)
{
  if (ui_char_button_list_filtered.size() == 0)
    return;

  QPoint f_spacing =
      ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  char_columns =
      ((ui_char_buttons->width() - button_width) / (x_spacing + button_width)) +
      1;
  char_rows = ((ui_char_buttons->height() - button_height) /
               (y_spacing + button_height)) +
              1;

  max_chars_on_page = char_columns * char_rows;

  int startout = starting;
  for (int n = starting; n < startout + chars_on_this_page; ++n) {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

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
  }

  // First, we'll make all the character buttons in the very beginning.
  // We also hide them all, so they can't be accidentally clicked.
  // Later on, we'll be revealing buttons as we need them.
  for (int n = 0; n < char_list.size(); n++) {
    AOCharButton *char_button =
        new AOCharButton(ui_char_buttons, ao_app, 0, 0, char_list.at(n).taken);
    char_button->reset();
    char_button->hide();
    char_button->set_image(char_list.at(n).name);
    char_button->setToolTip(char_list.at(n).name);
    ui_char_button_list.append(char_button);

    connect(char_button, &AOCharButton::clicked,
            [this, n]() { this->char_clicked(n); });

    // This part here serves as a way of showing to the player that the game is
    // still running, it is just loading the pictures of the characters.
    if (ao_app->lobby_constructed) {
      ao_app->generated_chars++;
      int total_loading_size = ao_app->char_list_size * 2 +
                               ao_app->evidence_list_size +
                               ao_app->music_list_size;
      int loading_value =
          int(((ao_app->loaded_chars + ao_app->generated_chars +
                ao_app->loaded_music + ao_app->loaded_evidence) /
               static_cast<double>(total_loading_size)) *
              100);
      ao_app->w_lobby->set_loading_value(loading_value);
      ao_app->w_lobby->set_loading_text(
          tr("Generating chars:\n%1/%2")
              .arg(QString::number(ao_app->generated_chars))
              .arg(QString::number(ao_app->char_list_size)));
    }
  }

  filter_character_list();
}

void Courtroom::filter_character_list()
{
  ui_char_button_list_filtered.clear();
  for (int i = 0; i < char_list.size(); i++) {
    AOCharButton *current_char = ui_char_button_list.at(i);

    // It seems passwording characters is unimplemented yet?
    // Until then, this will stay here, I suppose.
    // if (ui_char_passworded->isChecked() && character_is_passworded??)
    //    continue;

    if (!ui_char_taken->isChecked() && char_list.at(i).taken)
      continue;

    if (!char_list.at(i).name.contains(ui_char_search->text(),
                                       Qt::CaseInsensitive))
      continue;

    // We only really need to update the fact that a character is taken
    // for the buttons that actually appear.
    // You'd also update the passwordedness and etc. here later.
    current_char->reset();
    current_char->set_taken(char_list.at(i).taken);

    ui_char_button_list_filtered.append(current_char);
  }

  current_char_page = 0;
  set_char_select_page();
}

void Courtroom::on_char_search_changed() { filter_character_list(); }

void Courtroom::on_char_passworded_clicked() { filter_character_list(); }

void Courtroom::on_char_taken_clicked() { filter_character_list(); }
