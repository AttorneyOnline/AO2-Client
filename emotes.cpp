#include "courtroom.h"

#include "aoemotebutton.h"

void Courtroom::construct_emotes()
{
  //constructing emote button grid
  const int base_x_pos{10};
  const int base_y_pos{0};

  const int x_modifier{49};
  int x_mod_count{0};

  const int y_modifier{49};
  int y_mod_count{0};

  for (int n = 0 ; n < 90 ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    ui_emote_list.append(new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos));

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
  ui_emote_left->hide();
  ui_emote_right->hide();

  for (AOEmoteButton *i_button : ui_emote_list)
  {
    i_button->hide();
  }

}
