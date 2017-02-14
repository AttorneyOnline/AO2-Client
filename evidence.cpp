#include "courtroom.h"

void Courtroom::construct_evidence()
{
  //constructing evidence button grid
  const int base_x_pos{10};
  const int base_y_pos{0};

  const int x_modifier{49};
  int x_mod_count{0};

  const int y_modifier{49};
  int y_mod_count{0};

  evidence_columns = ui_evidence->width() / x_modifier;
  evidence_rows = ui_evidence->height() / y_modifier;

  max_evidence_on_page = evidence_columns * evidence_rows;

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
    if ((n % emote_columns) == (emote_columns - 1) && (n != 0))
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}
