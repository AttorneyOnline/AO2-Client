#include "courtroom.h"

#include "aoemotebutton.h"

#include <QDebug>

void Courtroom::construct_emotes()
{
  //constructing emote button grid
  const int base_x_pos{10};
  const int base_y_pos{0};

  const int x_modifier{49};
  int x_mod_count{0};

  const int y_modifier{49};
  int y_mod_count{0};

  for (int n = 0 ; n < 10 ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    AOEmoteButton *f_emote = new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos);

    ui_emote_list.append(f_emote);

    f_emote->set_id(n);

    connect(f_emote, SIGNAL(emote_clicked(int)), this, SLOT(on_emote_clicked(int)));

    ++x_mod_count;

    //if char number is divisible by 5 with rest 4 then the next emote button should start on a new line
    if (n % 5 == 4 && n != 0)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}

void Courtroom::set_emote_page()
{
  int total_emotes = ao_app->get_emote_number(current_char);

  ui_emote_left->hide();
  ui_emote_right->hide();

  for (AOEmoteButton *i_button : ui_emote_list)
  {
    i_button->hide();
  }

  int total_pages = total_emotes / 10;
  int emotes_on_page = 0;

  if (total_emotes % 10 != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_emote_page + 1)
      emotes_on_page = 10;
    else
      emotes_on_page = total_emotes % 10;

  }
  else
    emotes_on_page = 10;

  if (total_pages > current_emote_page + 1)
    ui_emote_right->show();

  if (current_emote_page > 0)
    ui_emote_left->show();

  for (int n_emote = 0 ; n_emote < emotes_on_page ; ++n_emote)
  {
    int n_real_emote = n_emote + current_emote_page * 10;
    AOEmoteButton *f_emote = ui_emote_list.at(n_emote);

    if (n_real_emote == current_emote)
      f_emote->set_on(current_char, n_real_emote);
    else
      f_emote->set_off(current_char, n_real_emote);

    f_emote->show();
  }

}

void Courtroom::on_emote_clicked(int p_id)
{
  current_emote = p_id + 10 * current_emote_page;

  for (int n_emote = 0 ; n_emote < 10 ; ++n_emote)
  {
    int n_real_emote = n_emote + current_emote_page * 10;
    AOEmoteButton *f_emote = ui_emote_list.at(n_emote);

    if (n_real_emote == current_emote)
      f_emote->set_on(current_char, n_real_emote);
    else
      f_emote->set_off(current_char, n_real_emote);
  }

  qDebug() << "on_emote_clicked with current_emote " << current_emote;

  int emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  qDebug() << "on_emote_clicked called with emote mod " << emote_mod;

  if (emote_mod == 1 ||
      emote_mod == 3 ||
      emote_mod == 4)
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
