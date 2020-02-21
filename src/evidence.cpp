#include "courtroom.h"

void Courtroom::construct_evidence()
{
  ui_evidence = new AOImage(this, ao_app);

  //ui_evidence_name = new QLabel(ui_evidence);
  ui_evidence_name = new AOLineEdit(ui_evidence);
  ui_evidence_name->setAlignment(Qt::AlignCenter);
  ui_evidence_name->setFont(QFont("Arial", 14, QFont::Bold));
  ui_evidence_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                  "color: rgba(255, 128, 0, 255);");

  ui_evidence_buttons = new QWidget(ui_evidence);

  ui_evidence_left = new AOButton(ui_evidence, ao_app);
  ui_evidence_right = new AOButton(ui_evidence, ao_app);
  ui_evidence_present = new AOButton(ui_evidence, ao_app);

  ui_evidence_overlay = new AOImage(ui_evidence, ao_app);

  ui_evidence_delete = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_image_name = new AOLineEdit(ui_evidence_overlay);
  ui_evidence_image_button = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_image_button->setText(tr("Choose..."));
  ui_evidence_x = new AOButton(ui_evidence_overlay, ao_app);

  ui_evidence_description = new AOTextEdit(ui_evidence_overlay);
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
    connect(f_evidence, SIGNAL(evidence_double_clicked(int)), this, SLOT(on_evidence_double_clicked(int)));
    connect(f_evidence, SIGNAL(on_hover(int, bool)), this, SLOT(on_evidence_hover(int, bool)));

    ++x_mod_count;

    if (x_mod_count == evidence_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  connect(ui_evidence_name, SIGNAL(returnPressed()), this, SLOT(on_evidence_name_edited()));
  connect(ui_evidence_left, SIGNAL(clicked()), this, SLOT(on_evidence_left_clicked()));
  connect(ui_evidence_right, SIGNAL(clicked()), this, SLOT(on_evidence_right_clicked()));
  connect(ui_evidence_present, SIGNAL(clicked()), this, SLOT(on_evidence_present_clicked()));
  connect(ui_evidence_delete, SIGNAL(clicked()), this, SLOT(on_evidence_delete_clicked()));
  connect(ui_evidence_image_name, SIGNAL(returnPressed()), this, SLOT(on_evidence_image_name_edited()));
  connect(ui_evidence_image_button, SIGNAL(clicked()), this, SLOT(on_evidence_image_button_clicked()));
  connect(ui_evidence_x, SIGNAL(clicked()), this, SLOT(on_evidence_x_clicked()));

  ui_evidence->hide();
}

void Courtroom::set_evidence_list(QVector<evi_type> &p_evi_list)
{
  local_evidence_list.clear();
  local_evidence_list = p_evi_list;

  set_evidence_page();
}

void Courtroom::set_evidence_page()
{
  int total_evidence = local_evidence_list.size();

  ui_evidence_left->hide();
  ui_evidence_right->hide();

  for (AOEvidenceButton *i_button : ui_evidence_list)
  {
    i_button->reset();
  }

  //to account for the "add evidence" button
  ++total_evidence;

  int total_pages = total_evidence / max_evidence_on_page;
  int evidence_on_page = 0;

  if ((total_evidence % max_evidence_on_page) != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_evidence_page + 1)
      evidence_on_page = max_evidence_on_page;
    else
      evidence_on_page = total_evidence % max_evidence_on_page;

  }
  else
    evidence_on_page = max_evidence_on_page;

  if (total_pages > current_evidence_page + 1)
    ui_evidence_right->show();

  if (current_evidence_page > 0)
    ui_evidence_left->show();

  for (int n_evidence_button = 0 ; n_evidence_button < evidence_on_page ; ++n_evidence_button)
  {
    int n_real_evidence = n_evidence_button + current_evidence_page * max_evidence_on_page;
    AOEvidenceButton *f_evidence_button = ui_evidence_list.at(n_evidence_button);

    //ie. the add evidence button
    if (n_real_evidence == (total_evidence - 1))
      f_evidence_button->set_theme_image("addevidence.png");
    else if (n_real_evidence < (total_evidence - 1))
    {
      f_evidence_button->set_image(local_evidence_list.at(n_real_evidence).image);

      if (n_real_evidence == current_evidence)
        f_evidence_button->set_selected(true);
      else
        f_evidence_button->set_selected(false);
    }
    else
      f_evidence_button->set_image("");

    f_evidence_button->show();
  }
}

void Courtroom::on_evidence_name_edited()
{
  if (current_evidence >= local_evidence_list.size())
    return;

  QStringList f_contents;

  evi_type f_evi = local_evidence_list.at(current_evidence);

  f_contents.append(QString::number(current_evidence));
  f_contents.append(ui_evidence_name->text());
  f_contents.append(f_evi.description);
  f_contents.append(f_evi.image);

  ao_app->send_server_packet(new AOPacket("EE", f_contents));
}

void Courtroom::on_evidence_image_name_edited()
{
  if (current_evidence >= local_evidence_list.size())
    return;

  QStringList f_contents;

  evi_type f_evi = local_evidence_list.at(current_evidence);

  f_contents.append(QString::number(current_evidence));
  f_contents.append(f_evi.name);
  f_contents.append(f_evi.description);
  f_contents.append(ui_evidence_image_name->text());

  ao_app->send_server_packet(new AOPacket("EE", f_contents));
}

void Courtroom::on_evidence_image_button_clicked()
{
  QDir dir(ao_app->get_base_path() + "evidence");
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("Images (*.png)"));
  dialog.setViewMode(QFileDialog::List);
  dialog.setDirectory(dir);

  QStringList filenames;

  if (dialog.exec())
      filenames = dialog.selectedFiles();

  if (filenames.size() != 1)
    return;

  QString filename = filenames.at(0);
  filename = dir.relativeFilePath(filename);
  ui_evidence_image_name->setText(filename);
  on_evidence_image_name_edited();
}

void Courtroom::on_evidence_clicked(int p_id)
{
  ui_evidence_name->setReadOnly(true);

  int f_real_id = p_id + max_evidence_on_page * current_evidence_page;

  if (f_real_id == local_evidence_list.size())
  {
    ao_app->send_server_packet(new AOPacket("PE#<name>#<description>#empty.png#%"));
    return;
  }
  else if (f_real_id > local_evidence_list.size())
    return;

  ui_evidence_name->setText(local_evidence_list.at(f_real_id).name);

  for (AOEvidenceButton *i_button : ui_evidence_list)
    i_button->set_selected(false);

  ui_evidence_list.at(p_id)->set_selected(true);

  current_evidence = f_real_id;

  ui_ic_chat_message->setFocus();

}

void Courtroom::on_evidence_double_clicked(int p_id)
{
  int f_real_id = p_id + max_evidence_on_page * current_evidence_page;

  if (f_real_id >= local_evidence_list.size())
    return;

  current_evidence = f_real_id;

  evi_type f_evi = local_evidence_list.at(f_real_id);

  ui_evidence_description->clear();
  ui_evidence_description->appendPlainText(f_evi.description);

  ui_evidence_image_name->setText(f_evi.image);

  ui_evidence_overlay->show();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_hover(int p_id, bool p_state)
{
  ui_evidence_name->setReadOnly(true);
  int final_id = p_id + max_evidence_on_page * current_evidence_page;

  if (p_state)
  {
    if (final_id == local_evidence_list.size())
      ui_evidence_name->setText(tr("Add new evidence..."));
    else if (final_id < local_evidence_list.size())
      ui_evidence_name->setText(local_evidence_list.at(final_id).name);
  }
  else if (current_evidence < local_evidence_list.size())
    ui_evidence_name->setText(local_evidence_list.at(current_evidence).name);
  else
    ui_evidence_name->setText("");
}

void Courtroom::on_evidence_left_clicked()
{
  --current_evidence_page;

  set_evidence_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_right_clicked()
{
  ++current_evidence_page;

  set_evidence_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_present_clicked()
{
  if (is_presenting_evidence)
    ui_evidence_present->set_image("present_disabled.png");
  else
    ui_evidence_present->set_image("present.png");

  is_presenting_evidence = !is_presenting_evidence;

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_delete_clicked()
{
  ui_evidence_description->setReadOnly(true);
  ui_evidence_overlay->hide();

  ao_app->send_server_packet(new AOPacket("DE#" + QString::number(current_evidence) + "#%"));

  current_evidence = 0;

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_x_clicked()
{
  ui_evidence_description->setReadOnly(true);
  ui_evidence_overlay->hide();

  if (current_evidence >= local_evidence_list.size())
    return;

  QStringList f_contents;

  evi_type f_evi = local_evidence_list.at(current_evidence);

  f_contents.append(QString::number(current_evidence));
  f_contents.append(f_evi.name);
  f_contents.append(ui_evidence_description->toPlainText());
  f_contents.append(f_evi.image);

  ao_app->send_server_packet(new AOPacket("EE", f_contents));

  ui_ic_chat_message->setFocus();
}

