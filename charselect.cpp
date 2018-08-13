#include "courtroom.h"

#include "file_functions.h"
#include "debug_functions.h"
#include "hardware_functions.h"

#include <QDebug>

void Courtroom::construct_char_select()
{
  ui_char_select_background = new AOImage(this, ao_app);

  ui_char_buttons = new QWidget(ui_char_select_background);

  ui_selector = new AOImage(ui_char_select_background, ao_app);
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->resize(62, 62);

  ui_back_to_lobby = new AOButton(ui_char_select_background, ao_app);

  ui_char_password = new QLineEdit(ui_char_select_background);
  ui_char_password->setPlaceholderText("Password");

  ui_char_select_left = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_right = new AOButton(ui_char_select_background, ao_app);

  ui_spectator = new AOButton(ui_char_select_background, ao_app);
  ui_spectator->setText("Spectator");

  ui_char_search = new QLineEdit(ui_char_select_background);
  ui_char_search->setPlaceholderText("Search");
  set_size_and_pos(ui_char_search, "char_search");

  ui_char_passworded = new QCheckBox(ui_char_select_background);
  ui_char_passworded->setText("Passworded");
  set_size_and_pos(ui_char_passworded, "char_passworded");

  ui_char_taken = new QCheckBox(ui_char_select_background);
  ui_char_taken->setText("Taken");
  set_size_and_pos(ui_char_taken, "char_taken");

  set_size_and_pos(ui_char_buttons, "char_buttons");

  connect (char_button_mapper, SIGNAL(mapped(int)), this, SLOT(char_clicked(int)));
  connect(ui_back_to_lobby, SIGNAL(clicked()), this, SLOT(on_back_to_lobby_clicked()));

  connect(ui_char_select_left, SIGNAL(clicked()), this, SLOT(on_char_select_left_clicked()));
  connect(ui_char_select_right, SIGNAL(clicked()), this, SLOT(on_char_select_right_clicked()));

  connect(ui_spectator, SIGNAL(clicked()), this, SLOT(on_spectator_clicked()));

  connect(ui_char_search, SIGNAL(textEdited(const QString&)), this, SLOT(on_char_search_changed(const QString&)));
  connect(ui_char_passworded, SIGNAL(stateChanged(int)), this, SLOT(on_char_passworded_clicked(int)));
  connect(ui_char_taken, SIGNAL(stateChanged(int)), this, SLOT(on_char_taken_clicked(int)));
}

void Courtroom::set_char_select()
{
  QString filename = "courtroom_design.ini";

  pos_size_type f_charselect = ao_app->get_element_dimensions("char_select", filename);

  if (f_charselect.width < 0 || f_charselect.height < 0)
  {
    qDebug() << "W: did not find courtroom width or height in courtroom_design.ini!";
    this->resize(714, 668);
  }
  else
    this->resize(f_charselect.width, f_charselect.height);

  ui_char_select_background->resize(f_charselect.width, f_charselect.height);
  ui_char_select_background->set_image("charselect_background.png");
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_char_select_left->hide();
  ui_char_select_right->hide();

  for (AOCharButton *i_button : ui_char_button_list)
  {
    i_button->hide();
    i_button->move(0,0);
  }

  int total_pages = ui_char_button_list_filtered.size() / max_chars_on_page;
  int chars_on_page = 0;

  if (ui_char_button_list_filtered.size() % max_chars_on_page != 0)
  {
    ++total_pages;
    //i. e. not on the last page
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
  QString char_ini_path = ao_app->get_character_path(char_list.at(n_char).name) + "char.ini";
  qDebug() << "char_ini_path" << char_ini_path;

  if (!file_exists(char_ini_path))
  {
    qDebug() << "did not find " << char_ini_path;
    call_notice("Could not find " + char_ini_path);
    return;
  }

  if (n_char == m_cid)
  {
    enter_courtroom(m_cid);
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("CC#" + QString::number(ao_app->s_pv) + "#" + QString::number(n_char) + "#" + get_hdid() + "#%"));
  }

  ui_ic_chat_name->setPlaceholderText(char_list.at(n_char).name);
}

void Courtroom::put_button_in_place(int starting, int chars_on_this_page)
{
    if (ui_char_button_list_filtered.size() == 0)
        return;

    QPoint f_spacing = ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

    int x_spacing = f_spacing.x();
    int x_mod_count = 0;

    int y_spacing = f_spacing.y();
    int y_mod_count = 0;

    char_columns = ((ui_char_buttons->width() - button_width) / (x_spacing + button_width)) + 1;
    char_rows = ((ui_char_buttons->height() - button_height) / (y_spacing + button_height)) + 1;

    int startout = starting;
    for (int n = starting ; n < startout+chars_on_this_page ; ++n)
    {
      int x_pos = (button_width + x_spacing) * x_mod_count;
      int y_pos = (button_height + y_spacing) * y_mod_count;

      ui_char_button_list_filtered.at(n)->move(x_pos, y_pos);
      ui_char_button_list_filtered.at(n)->show();

      ++x_mod_count;

      if (x_mod_count == char_columns)
      {
        ++y_mod_count;
        x_mod_count = 0;
      }
    }
}

void Courtroom::character_loading_finished()
{
    // First, we'll make all the character buttons in the very beginning.
    // We also hide them all, so they can't be accidentally clicked.
    // Later on, we'll be revealing buttons as we need them.
    for (int n = 0; n < char_list.size(); n++)
    {
      AOCharButton* character = new AOCharButton(ui_char_buttons, ao_app, 0, 0);
      character->hide();
      character->set_image(char_list.at(n).name);
      ui_char_button_list.append(character);

      connect(character, SIGNAL(clicked()), char_button_mapper, SLOT(map()));
      char_button_mapper->setMapping(character, ui_char_button_list.size() - 1);
    }

    filter_character_list();
    put_button_in_place(0, max_chars_on_page);
}

void Courtroom::filter_character_list()
{
    ui_char_button_list_filtered.clear();
    for (int i = 0; i < char_list.size(); i++)
    {
      AOCharButton* current_char = ui_char_button_list.at(i);

      // It seems passwording characters is unimplemented yet?
      // Until then, this will stay here, I suppose.
      //if (ui_char_passworded->isChecked() && character_is_passworded??)
      //    continue;

      if (!ui_char_taken->isChecked() && char_list.at(i).taken)
          continue;

      if (!char_list.at(i).name.contains(ui_char_search->text(), Qt::CaseInsensitive))
          continue;

      ui_char_button_list_filtered.append(current_char);
    }

    current_char_page = 0;
    set_char_select_page();
}

void Courtroom::on_char_search_changed(const QString& newtext)
{
    filter_character_list();
}

void Courtroom::on_char_passworded_clicked(int newstate)
{
    filter_character_list();
}

void Courtroom::on_char_taken_clicked(int newstate)
{
    filter_character_list();
}
