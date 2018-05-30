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

  ui_char_select_left = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_right = new AOButton(ui_char_select_background, ao_app);

  ui_spectator = new AOButton(ui_char_select_background, ao_app);
  ui_spectator->setText(tr("Spectator"));

  QPoint f_spacing = ao_app->get_button_spacing("char_button_spacing", "courtroom_design.ini");

  const int button_width = 60;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = 60;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  set_size_and_pos(ui_char_buttons, "char_buttons");

  char_columns = ((ui_char_buttons->width() - button_width) / (x_spacing + button_width)) + 1;
  char_rows = ((ui_char_buttons->height() - button_height) / (y_spacing + button_height)) + 1;

  max_chars_on_page = char_columns * char_rows;

  for (int n = 0 ; n < max_chars_on_page ; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    ui_char_button_list.append(new AOCharButton(ui_char_buttons, ao_app, x_pos, y_pos));

    connect(ui_char_button_list.at(n), SIGNAL(clicked()), char_button_mapper, SLOT(map())) ;
    char_button_mapper->setMapping (ui_char_button_list.at(n), n) ;

    ++x_mod_count;

    if (x_mod_count == char_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  connect (char_button_mapper, SIGNAL(mapped(int)), this, SLOT(char_clicked(int)));
  connect(ui_back_to_lobby, SIGNAL(clicked()), this, SLOT(on_back_to_lobby_clicked()));

  connect(ui_char_select_left, SIGNAL(clicked()), this, SLOT(on_char_select_left_clicked()));
  connect(ui_char_select_right, SIGNAL(clicked()), this, SLOT(on_char_select_right_clicked()));

  connect(ui_spectator, SIGNAL(clicked()), this, SLOT(on_spectator_clicked()));
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
    i_button->hide();

  int total_pages = char_list.size() / max_chars_on_page;
  int chars_on_page = 0;

  if (char_list.size() % max_chars_on_page != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_char_page + 1)
      chars_on_page = max_chars_on_page;
    else
      chars_on_page = char_list.size() % max_chars_on_page;

  }
  else
    chars_on_page = max_chars_on_page;

  if (total_pages > current_char_page + 1)
    ui_char_select_right->show();

  if (current_char_page > 0)
    ui_char_select_left->show();

  for (int n_button = 0 ; n_button < chars_on_page ; ++n_button)
  {
    int n_real_char = n_button + current_char_page * max_chars_on_page;
    AOCharButton *f_button = ui_char_button_list.at(n_button);

    f_button->reset();
    f_button->set_image(char_list.at(n_real_char).name);
    f_button->show();

    if (char_list.at(n_real_char).taken)
      f_button->set_taken();
  }

}

void Courtroom::char_clicked(int n_char)
{
  int n_real_char = n_char + current_char_page * max_chars_on_page;

  QString char_ini_path = ao_app->get_character_path(char_list.at(n_real_char).name) + "char.ini";
  qDebug() << "char_ini_path" << char_ini_path;

  if (!file_exists(char_ini_path))
  {
    qDebug() << "did not find " << char_ini_path;
    call_notice(tr("Could not find %1").arg(char_ini_path));
    return;
  }

  if (n_real_char == m_cid)
  {
    enter_courtroom(m_cid);
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("CC#" + QString::number(ao_app->s_pv) + "#" + QString::number(n_real_char) + "#" + get_hdid() + "#%"));
  }
}

