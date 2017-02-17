#include "courtroom.h"

#include "aoemotebutton.h"

#include <QDebug>

void Courtroom::construct_emotes()
{
  //constructing emote button grid
  const int base_x_pos{0};
  const int base_y_pos{0};

  const int x_modifier{49};
  int x_mod_count{0};

  const int y_modifier{49};
  int y_mod_count{0};

  emote_columns = ui_emotes->width() / x_modifier;
  emote_rows = ui_emotes->height() / y_modifier;

  max_emotes_on_page = emote_columns * emote_rows;

  for (int n = 0 ; n < max_emotes_on_page ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    AOEmoteButton *f_emote = new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos);

    ui_emote_list.append(f_emote);

    f_emote->set_id(n);

    connect(f_emote, SIGNAL(emote_clicked(int)), this, SLOT(on_emote_clicked(int)));

    ++x_mod_count;

    //if emote number is divisible by columns with rest columns -1 then the next emote button should start on a new line
    if (x_mod_count == emote_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  connect(ui_emote_left, SIGNAL(clicked()), this, SLOT(on_emote_left_clicked()));
  connect(ui_emote_right, SIGNAL(clicked()), this, SLOT(on_emote_right_clicked()));
}

void Courtroom::set_emote_page()
{
  if (m_cid == -1)
    return;

  int total_emotes = ao_app->get_emote_number(current_char);

  ui_emote_left->hide();
  ui_emote_right->hide();

  for (AOEmoteButton *i_button : ui_emote_list)
  {
    i_button->hide();
  }

  int total_pages = total_emotes / max_emotes_on_page;
  int emotes_on_page = 0;

  if (total_emotes % max_emotes_on_page != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_emote_page + 1)
      emotes_on_page = max_emotes_on_page;
    else
      emotes_on_page = total_emotes % max_emotes_on_page;

  }
  else
    emotes_on_page = max_emotes_on_page;

  if (total_pages > current_emote_page + 1)
    ui_emote_right->show();

  if (current_emote_page > 0)
    ui_emote_left->show();

  for (int n_emote = 0 ; n_emote < emotes_on_page ; ++n_emote)
  {
    int n_real_emote = n_emote + current_emote_page * max_emotes_on_page;
    AOEmoteButton *f_emote = ui_emote_list.at(n_emote);

    if (n_real_emote == current_emote)
      f_emote->set_image(current_char, n_real_emote, "_on.png");
    else
      f_emote->set_image(current_char, n_real_emote, "_off.png");

    f_emote->show();
  }

}

void Courtroom::set_emote_dropdown()
{
  ui_emote_dropdown->clear();

  int total_emotes = ao_app->get_emote_number(current_char);
  QStringList emote_list;

  for (int n = 0 ; n < total_emotes ; ++n)
  {
    emote_list.append(ao_app->get_emote_comment(current_char, n));
  }

  ui_emote_dropdown->addItems(emote_list);
}

void Courtroom::on_emote_clicked(int p_id)
{
  int min = current_emote_page * max_emotes_on_page;
  int max = (max_emotes_on_page - 1) + current_emote_page * max_emotes_on_page;

  if (current_emote >= min && current_emote <= max)
    ui_emote_list.at(current_emote % max_emotes_on_page)->set_image(current_char, current_emote, "_off.png");

  int old_emote = current_emote;

  current_emote = p_id + max_emotes_on_page * current_emote_page;

  if (current_emote >= min && current_emote <= max)
    ui_emote_list.at(current_emote % max_emotes_on_page)->set_image(current_char, current_emote, "_on.png");

  int emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  if (old_emote == current_emote)
  {
    ui_pre->setChecked(!ui_pre->isChecked());
  }
  else if (emote_mod == 1)
    ui_pre->setChecked(true);
  else
    ui_pre->setChecked(false);

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_left_clicked()
{
  --current_emote_page;

  set_emote_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_right_clicked()
{
  ++current_emote_page;

  set_emote_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_dropdown_changed(int p_index)
{
  on_emote_clicked(p_index);
}
