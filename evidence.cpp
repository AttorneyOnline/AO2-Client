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
  evidence_rows = ui_evidence->height() / y_modifier;

  max_evidence_on_page = evidence_columns * evidence_rows;

  for (int n = 0 ; n < max_evidence_on_page ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    AOEvidenceButton *f_evidence = new AOEvidenceButton(ui_evidence, ao_app, x_pos, y_pos);

    ui_evidence_list.append(f_evidence);

    f_evidence->set_id(n);

    connect(f_evidence, SIGNAL(evidence_clicked(int)), this, SLOT(on_evidence_clicked(int)));
    connect(f_evidence, SIGNAL(on_hover(int, bool)), this, SLOT(on_evidence_hover(int, bool)));

    ++x_mod_count;

    if (x_mod_count == evidence_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  ui_evidence_name = new QLabel(ui_evidence);

  ui_evidence_left = new AOImage(ui_evidence, ao_app);
  ui_evidence_right = new AOImage(ui_evidence, ao_app);

  ui_evidence_overlay = new AOImage(ui_evidence, ao_app);

  ui_evidence_x = new AOButton(ui_evidence_overlay, ao_app);

  ui_evidence_description = new QPlainTextEdit(ui_evidence_overlay);

  set_size_and_pos(ui_evidence_name, "evidence_name");
  ui_evidence_name->setAlignment(Qt::AlignCenter);
  ui_evidence_name->setFont(QFont("Arial", 14, QFont::Bold));
  ui_evidence_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                  "color: rgba(255, 128, 0, 255);");

  set_size_and_pos(ui_evidence_left, "evidence_left");
  ui_evidence_left->set_image("arrow_left.png");

  set_size_and_pos(ui_evidence_right, "evidence_right");
  ui_evidence_right->set_image("arrow_right.png");

  set_size_and_pos(ui_evidence_overlay, "evidence_overlay");
  ui_evidence_overlay->set_image("evidenceoverlay.png");

  set_size_and_pos(ui_evidence_x, "evidence_x");
  ui_evidence_x->setText("X");

  set_size_and_pos(ui_evidence_description, "evidence_description");
  ui_evidence_description->setReadOnly(true);
  ui_evidence_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                         "color: white;");

  connect(ui_evidence_x, SIGNAL(clicked()), this, SLOT(on_evidence_x_clicked()));

  ui_evidence->hide();
}

void Courtroom::set_evidence_page()
{
  local_evidence_list.clear();

  QString evi_string = char_list.at(m_cid).evidence_string;

  QStringList evi_numbers = evi_string.split(",");

  for (QString i_evi : evi_numbers)
  {
    int n_evi = i_evi.toInt() - 1;

    if (n_evi < 0 || n_evi >= evidence_list.size())
      continue;

    local_evidence_list.append(evidence_list.at(n_evi));
  }

  int total_evidence = local_evidence_list.size();

  qDebug() << "total_evidence: " << total_evidence;

  ui_evidence_left->hide();
  ui_evidence_right->hide();

  qDebug() << "hid evidence left and right";

  for (AOEvidenceButton *i_button : ui_evidence_list)
  {
    i_button->hide();
  }

  qDebug() << "hid all evidence buttons";

  if (total_evidence == 0)
    return;

  qDebug() << "max_evi_on_page =" << max_evidence_on_page;

  int total_pages = total_evidence / max_evidence_on_page;

  qDebug() << "total_pages: " << total_pages;

  int evidence_on_page = 0;

  if (total_evidence % max_evidence_on_page != 0)
  {
    ++total_pages;
    evidence_on_page = total_evidence % max_evidence_on_page;

  }
  else
    evidence_on_page = max_evidence_on_page;

  qDebug() << "evidence_on_page: " << evidence_on_page;

  if (total_pages > current_evidence_page + 1)
    ui_evidence_right->show();

  if (current_evidence_page > 0)
    ui_evidence_left->show();

  for (int n_evidence = 0 ; n_evidence < evidence_on_page ; ++n_evidence)
  {
    int n_real_evidence = n_evidence + current_evidence_page * max_evidence_on_page;
    AOEvidenceButton *f_evidence = ui_evidence_list.at(n_evidence);

    f_evidence->set_image(evidence_list.at(n_real_evidence).image);

    f_evidence->show();
  }

}

void Courtroom::on_evidence_clicked(int p_id)
{
  ui_evidence_name->setText(local_evidence_list.at(p_id + max_evidence_on_page * current_evidence_page).name);

  for (AOEvidenceButton *i_button : ui_evidence_list)
  {
    i_button->set_selected(false);
  }

  ui_evidence_list.at(p_id)->set_selected(true);

  current_evidence = p_id + max_evidence_on_page * current_evidence_page;

  ui_evidence_description->clear();
  ui_evidence_description->appendPlainText(local_evidence_list.at(current_evidence).description);

  ui_evidence_overlay->show();
}

void Courtroom::on_evidence_hover(int p_id, bool p_state)
{
  if (p_state)
    ui_evidence_name->setText(local_evidence_list.at(p_id + max_evidence_on_page * current_evidence_page).name);
  else
    ui_evidence_name->setText("");
}

void Courtroom::on_evidence_left_clicked()
{
  --current_evidence_page;

  set_evidence_page();
}

void Courtroom::on_evidence_right_clicked()
{
  ++current_evidence_page;

  set_evidence_page();
}

void Courtroom::on_evidence_x_clicked()
{
  ui_evidence_overlay->hide();
}

