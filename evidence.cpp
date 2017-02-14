#include "courtroom.h"

#include <QDebug>

void Courtroom::construct_evidence()
{
  //constructing evidence button grid
  const int base_x_pos{28};
  const int base_y_pos{27};

  const int x_modifier{72};
  int x_mod_count{0};

  const int y_modifier{73};
  int y_mod_count{0};

  evidence_columns = ui_evidence->width() / x_modifier;
  qDebug() << "columns: " << evidence_columns;
  evidence_rows = ui_evidence->height() / y_modifier;
  qDebug() << "rows: " << evidence_rows;

  max_evidence_on_page = evidence_columns * evidence_rows;

  qDebug() << "max_evidence_on_page: " << max_evidence_on_page;

  for (int n = 0 ; n < max_evidence_on_page ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    AOEvidenceButton *f_evidence = new AOEvidenceButton(ui_evidence, ao_app, x_pos, y_pos);

    ui_evidence_list.append(f_evidence);

    f_evidence->set_id(n);

    //connect(f_evidence, SIGNAL(evidence_clicked(int)), this, SLOT(on_evidence_clicked(int)));

    ++x_mod_count;

    if (x_mod_count == evidence_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}
