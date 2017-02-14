#include "courtroom.h"

void Courtroom::construct_evidence()
{
  //constructing emote button grid
  const int base_x_pos{10};
  const int base_y_pos{0};

  const int x_modifier{49};
  int x_mod_count{0};

  const int y_modifier{49};
  int y_mod_count{0};

  emote_columns = ui_emotes->width() / x_modifier;
  emote_rows = ui_emotes->height() / y_modifier;

  max_emotes_on_page = emote_columns * emote_rows;
}
