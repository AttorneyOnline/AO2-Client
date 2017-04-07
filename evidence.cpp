#include "courtroom.h"

#include <QDebug>

void Courtroom::construct_evidence()
{
  ui_evidence = new AOImage(this, ao_app);

  ui_evidence_name = new QLabel(ui_evidence);
  ui_evidence_name->setAlignment(Qt::AlignCenter);
  ui_evidence_name->setFont(QFont("Arial", 14, QFont::Bold));
  ui_evidence_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                  "color: rgba(255, 128, 0, 255);");

  ui_evidence_buttons = new QWidget(ui_evidence);

  ui_evidence_left = new AOButton(ui_evidence, ao_app);
  ui_evidence_right = new AOButton(ui_evidence, ao_app);

  ui_evidence_overlay = new AOImage(ui_evidence, ao_app);

  ui_evidence_x = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_x->setText("X");

  ui_evidence_description = new QPlainTextEdit(ui_evidence_overlay);
  ui_evidence_description->setReadOnly(true);
  ui_evidence_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                         "color: white;");

  set_size_and_pos(ui_evidence, "evidence_background");
  set_size_and_pos(ui_evidence_buttons, "evidence_buttons");

  QPoint f_spacing = ao_app->get_button_spacing("evidence_button_spacing", "courtroom_design.ini");

  const int button_width = 70;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = 70;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  evidence_columns = ((ui_evidence_buttons->width() - button_width) / (x_spacing + button_width)) + 1;
  evidence_rows = ((ui_evidence_buttons->height() - button_height) / (y_spacing + button_height)) + 1;

  max_evidence_on_page = evidence_columns * evidence_rows;

  for (int n = 0 ; n < max_evidence_on_page ; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEvidenceButton *f_evidence = new AOEvidenceButton(ui_evidence_buttons, ao_app, x_pos, y_pos);

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

  connect(ui_evidence_left, SIGNAL(clicked()), this, SLOT(on_evidence_left_clicked()));
  connect(ui_evidence_right, SIGNAL(clicked()), this, SLOT(on_evidence_right_clicked()));
  connect(ui_evidence_x, SIGNAL(clicked()), this, SLOT(on_evidence_x_clicked()));

  ui_evidence->hide();
}

void Courtroom::set_evidence_page()
{
  if (m_cid == -1)
    return;

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

  ui_evidence_left->hide();
  ui_evidence_right->hide();

  for (AOEvidenceButton *i_button : ui_evidence_list)
  {
    i_button->hide();
  }

  if (total_evidence == 0)
    return;

  int total_pages = evidence_list.size() / max_evidence_on_page;
  int evidence_on_page = 0;

  if ((evidence_list.size() % max_evidence_on_page) != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_evidence_page + 1)
      evidence_on_page = max_evidence_on_page;
    else
      evidence_on_page = evidence_list.size() % max_evidence_on_page;

  }
  else
    evidence_on_page = max_evidence_on_page;

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

