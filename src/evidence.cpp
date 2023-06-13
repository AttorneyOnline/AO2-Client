#include "courtroom.h"
#include "options.h"

void Courtroom::initialize_evidence()
{
  ui_evidence = new AOImage(this, ao_app);
  ui_evidence->setObjectName("ui_evidence");

  // ui_evidence_name = new QLabel(ui_evidence);
  ui_evidence_name = new QLineEdit(ui_evidence);
  ui_evidence_name_filter = new AOLineEditFilter();
  ui_evidence_name->installEventFilter(ui_evidence_name_filter);
  ui_evidence_name->setAlignment(Qt::AlignCenter);
  ui_evidence_name->setFrame(false);
  ui_evidence_name->setObjectName("ui_evidence_name");
  ui_evidence_name->setReadOnly(true);

  ui_evidence_buttons = new QWidget(ui_evidence);
  ui_evidence_buttons->setObjectName("ui_evidence_buttons");

  ui_evidence_left = new AOButton(ui_evidence, ao_app);
  ui_evidence_left->setObjectName("ui_evidence_left");
  ui_evidence_right = new AOButton(ui_evidence, ao_app);
  ui_evidence_right->setObjectName("ui_evidence_right");
  ui_evidence_present = new AOButton(ui_evidence, ao_app);
  ui_evidence_present->setToolTip(tr("Present this piece of evidence to "
                                     "everyone on your next spoken message"));
  ui_evidence_present->setObjectName("ui_evidence_present");

  ui_evidence_switch = new AOButton(ui_evidence, ao_app);
  ui_evidence_switch->setObjectName("ui_evidence_switch");
  ui_evidence_transfer = new AOButton(ui_evidence, ao_app);
  ui_evidence_transfer->setObjectName("ui_evidence_transfer");

  ui_evidence_save = new AOButton(ui_evidence, ao_app);
  ui_evidence_save->setToolTip(tr("Save evidence to an .ini file."));
  ui_evidence_save->setObjectName("ui_evidence_save");
  ui_evidence_load = new AOButton(ui_evidence, ao_app);
  ui_evidence_load->setToolTip(tr("Load evidence from an .ini file."));
  ui_evidence_load->setObjectName("ui_evidence_load");

  ui_evidence_overlay = new AOImage(ui_evidence, ao_app);
  ui_evidence_overlay->setObjectName("ui_evidence_overlay");

  ui_evidence_delete = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_delete->setToolTip(tr("Destroy this piece of evidence"));
  ui_evidence_delete->setObjectName("ui_evidence_delete");
  ui_evidence_image_name = new QLineEdit(ui_evidence_overlay);
  ui_evidence_image_name_filter = new AOLineEditFilter();
  ui_evidence_image_name->installEventFilter(ui_evidence_image_name_filter);
  ui_evidence_image_name->setObjectName("ui_evidence_image_name");
  ui_evidence_image_button = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_image_button->setText(tr("Choose.."));
  ui_evidence_image_button->setObjectName("ui_evidence_image_button");
  ui_evidence_x = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_x->setToolTip(
      tr("Close the evidence display/editing overlay.\n"
         "You will be prompted if there's any unsaved changes."));
  ui_evidence_x->setObjectName("ui_evidence_x");
  ui_evidence_ok = new AOButton(ui_evidence_overlay, ao_app);
  ui_evidence_ok->setToolTip(tr("Save any changes made to this piece of "
                                "evidence and send them to server."));
  ui_evidence_ok->setObjectName("ui_evidence_ok");

  ui_evidence_description = new QPlainTextEdit(ui_evidence_overlay);
  ui_evidence_description->setFrameStyle(QFrame::NoFrame);
  ui_evidence_description->setToolTip(
      tr("Click to edit. Press [X] to update your changes."));
  ui_evidence_description->setObjectName("ui_evidence_description");

  connect(ui_evidence_name, &QLineEdit::returnPressed, this,
          &Courtroom::on_evidence_name_edited);
  connect(ui_evidence_left, &AOButton::clicked, this,
          &Courtroom::on_evidence_left_clicked);
  connect(ui_evidence_right, &AOButton::clicked, this,
          &Courtroom::on_evidence_right_clicked);
  connect(ui_evidence_present, &AOButton::clicked, this,
          &Courtroom::on_evidence_present_clicked);
  connect(ui_evidence_switch, &AOButton::clicked, this,
          &Courtroom::on_evidence_switch_clicked);
  connect(ui_evidence_transfer, &AOButton::clicked, this,
          &Courtroom::on_evidence_transfer_clicked);
  connect(ui_evidence_save, &AOButton::clicked, this,
          &Courtroom::on_evidence_save_clicked);
  connect(ui_evidence_load, &AOButton::clicked, this,
          &Courtroom::on_evidence_load_clicked);

  connect(ui_evidence_delete, &AOButton::clicked, this,
          &Courtroom::on_evidence_delete_clicked);
  connect(ui_evidence_image_name, &QLineEdit::returnPressed, this,
          &Courtroom::on_evidence_image_name_edited);
  connect(ui_evidence_image_button, &AOButton::clicked, this,
          &Courtroom::on_evidence_image_button_clicked);
  connect(ui_evidence_x, &AOButton::clicked, this,
          &Courtroom::on_evidence_x_clicked);
  connect(ui_evidence_ok, &AOButton::clicked, this,
          &Courtroom::on_evidence_ok_clicked);

  connect(ui_evidence_name, &QLineEdit::textChanged, this,
          &Courtroom::on_evidence_edited);
  connect(ui_evidence_image_name, &QLineEdit::textChanged, this,
          &Courtroom::on_evidence_edited);
  connect(ui_evidence_description, &QPlainTextEdit::textChanged, this,
          &Courtroom::on_evidence_edited);

  ui_evidence->hide();
  evidence_load("inventories/autosave.ini");
}

void Courtroom::refresh_evidence()
{
  set_font(ui_evidence_name, "", "evidence_name");
  set_font(ui_evidence_image_name, "", "evidence_image_name");
  set_font(ui_evidence_description, "", "evidence_description");

  // Should properly refresh the evidence list
  qDeleteAll(ui_evidence_list.begin(), ui_evidence_list.end());
  ui_evidence_list.clear();

  set_size_and_pos(ui_evidence_button, "evidence_button");
  ui_evidence_button->set_image("evidence_button");
  ui_evidence_button->setToolTip(tr("Bring up the Evidence screen."));

  set_size_and_pos(ui_evidence, "evidence_background");
  if (current_evidence_global)
    ui_evidence->set_image("evidence_background");
  else
    ui_evidence->set_image("evidence_background_private");

  set_size_and_pos(ui_evidence_name, "evidence_name");

  set_size_and_pos(ui_evidence_buttons, "evidence_buttons");

  set_size_and_pos(ui_evidence_left, "evidence_left");
  ui_evidence_left->set_image("arrow_left");

  set_size_and_pos(ui_evidence_right, "evidence_right");
  ui_evidence_right->set_image("arrow_right");

  set_size_and_pos(ui_evidence_present, "evidence_present");
  ui_evidence_present->set_image("present");

  set_size_and_pos(ui_evidence_overlay, "evidence_overlay");
  if (current_evidence_global)
    ui_evidence_overlay->set_image("evidence_overlay");
  else
    ui_evidence_overlay->set_image("evidence_overlay_private");

  set_size_and_pos(ui_evidence_delete, "evidence_delete");
  ui_evidence_delete->set_image("evidence_delete");

  set_size_and_pos(ui_evidence_image_name, "evidence_image_name");

  set_size_and_pos(ui_evidence_image_button, "evidence_image_button");

  set_size_and_pos(ui_evidence_x, "evidence_x");
  ui_evidence_x->set_image("evidence_x");

  set_size_and_pos(ui_evidence_ok, "evidence_ok");
  ui_evidence_ok->set_image("evidence_ok");

  set_size_and_pos(ui_evidence_switch, "evidence_switch");
  if (current_evidence_global) {
    ui_evidence_switch->set_image("evidence_global");
    ui_evidence_switch->setToolTip(tr("Switch evidence to private inventory."));
  }
  else {
    ui_evidence_switch->set_image("evidence_private");
    ui_evidence_switch->setToolTip(tr("Switch evidence to global inventory."));
  }

  set_size_and_pos(ui_evidence_transfer, "evidence_transfer");
  if (current_evidence_global) {
    ui_evidence_transfer->set_image("evidence_transfer");
    ui_evidence_transfer->setToolTip(
        tr("Transfer evidence to private inventory."));
  }
  else {
    ui_evidence_transfer->set_image("evidence_transfer_private");
    ui_evidence_transfer->setToolTip(
        tr("Transfer evidence to global inventory."));
  }

  set_size_and_pos(ui_evidence_save, "evidence_save");
  ui_evidence_save->set_image("evidence_save");
  if (current_evidence_global)
    ui_evidence_save->hide();
  else
    ui_evidence_save->show();

  set_size_and_pos(ui_evidence_load, "evidence_load");
  ui_evidence_load->set_image("evidence_load");
  if (current_evidence_global)
    ui_evidence_load->hide();
  else
    ui_evidence_load->show();

  set_size_and_pos(ui_evidence_description, "evidence_description");

  QPoint f_spacing = ao_app->get_button_spacing("evidence_button_spacing",
                                                "courtroom_design.ini");
  QPoint p_point = ao_app->get_button_spacing("evidence_button_size",
                                              "courtroom_design.ini");

  const int button_width = p_point.x();
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = p_point.y();
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  evidence_columns = ((ui_evidence_buttons->width() - button_width) /
                      (x_spacing + button_width)) +
                     1;
  evidence_rows = ((ui_evidence_buttons->height() - button_height) /
                   (y_spacing + button_height)) +
                  1;

  max_evidence_on_page = evidence_columns * evidence_rows;

  for (int n = 0; n < max_evidence_on_page; ++n) {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEvidenceButton *f_evidence = new AOEvidenceButton(
        ui_evidence_buttons, ao_app, x_pos, y_pos, button_width, button_height);

    ui_evidence_list.append(f_evidence);

    f_evidence->set_id(n);

    connect(f_evidence, &AOEvidenceButton::evidence_clicked, this,
            &Courtroom::on_evidence_clicked);
    connect(f_evidence, &AOEvidenceButton::evidence_double_clicked, this,
            &Courtroom::on_evidence_double_clicked);
    connect(f_evidence, &AOEvidenceButton::on_hover, this,
            &Courtroom::on_evidence_hover);

    ++x_mod_count;

    if (x_mod_count == evidence_columns) {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}

void Courtroom::set_evidence_list(QVector<evi_type> &p_evi_list)
{
  global_evidence_list = p_evi_list;
  if (!current_evidence_global)
    return; // We're on private evidence editing, wait for user to do their
            // thing

  QVector<evi_type> old_list = local_evidence_list;
  local_evidence_list.clear();
  local_evidence_list = p_evi_list;

  set_evidence_page();

  if (ui_evidence_overlay
          ->isVisible()) // Update the currently edited evidence for this user
  {
    int p_id = current_evidence - (max_evidence_on_page * current_evidence_page);
    if (current_evidence >= local_evidence_list.size()) {
      evidence_close();
      ui_evidence_name->setText("");
    }
    else if (ui_evidence_ok->isHidden()) // We haven't clicked to edit it or anything
    {
      on_evidence_double_clicked(p_id);
    }
    // Todo: make a function that compares two pieces of evidence for any
    // differences
    else if (compare_evidence_changed(
                 old_list.at(current_evidence),
                 local_evidence_list.at(current_evidence))) {
      QMessageBox *msgBox = new QMessageBox;

      msgBox->setAttribute(Qt::WA_DeleteOnClose);
      msgBox->setText(tr("The piece of evidence you've been editing has changed."));
      msgBox->setInformativeText(tr("Do you wish to keep your changes?"));
      msgBox->setDetailedText(tr(
          "Name: %1\n"
          "Image: %2\n"
          "Description:\n%3").arg(
                                    local_evidence_list.at(current_evidence).name,
                                    local_evidence_list.at(current_evidence).image,
                                    local_evidence_list.at(current_evidence).description)
                                  );
      msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox->setDefaultButton(QMessageBox::LastButton);
      // msgBox->setWindowModality(Qt::NonModal);
      int ret = msgBox->exec();
      switch (ret) {
      case QMessageBox::Yes:
        // "Keep changes"
        ui_evidence_ok->hide();
        break;
      case QMessageBox::No:
        // "Discard changes and keep theirs"
        on_evidence_double_clicked(p_id);
        break;
      default:
        // should never be reached
        break;
      }
    }
  }
}

void Courtroom::set_evidence_page()
{
  int total_evidence = local_evidence_list.size();

  ui_evidence_left->hide();
  ui_evidence_right->hide();

  for (AOEvidenceButton *i_button : qAsConst(ui_evidence_list)) {
    i_button->hide();
  }

  // to account for the "add evidence" button
  ++total_evidence;

  int total_pages = total_evidence / max_evidence_on_page;
  int evidence_on_page = 0;

  if ((total_evidence % max_evidence_on_page) != 0) {
    ++total_pages;
    // i. e. not on the last page
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

  for (int n_evidence_button = 0; n_evidence_button < evidence_on_page;
       ++n_evidence_button) {
    int n_real_evidence =
        n_evidence_button + current_evidence_page * max_evidence_on_page;
    AOEvidenceButton *f_evidence_button =
        ui_evidence_list.at(n_evidence_button);

    f_evidence_button->set_selected(false);
    f_evidence_button->setToolTip("");
    if (n_real_evidence == (total_evidence - 1)) {
      f_evidence_button->set_theme_image("addevidence.png");
    }
    else if (n_real_evidence < (total_evidence - 1)) {
      f_evidence_button->set_image(
          local_evidence_list.at(n_real_evidence).image);

      if (n_real_evidence == current_evidence)
        f_evidence_button->set_selected(true);

      f_evidence_button->setToolTip(
          QString::number(n_real_evidence + 1) + ": " +
          local_evidence_list.at(n_real_evidence).name);
    }
    else
      f_evidence_button->set_image("");

    f_evidence_button->show();
  }
}

void Courtroom::show_evidence(int f_real_id)
{
  // Make sure we're in the global evidence list
  evidence_switch(true);
  // Set the evidence page properly
  current_evidence_page = f_real_id / max_evidence_on_page;
  set_evidence_page();
  // Display the target evidence using the local ID
  int p_id = f_real_id - (max_evidence_on_page * current_evidence_page);
  on_evidence_double_clicked(p_id);
}


void Courtroom::on_evidence_name_edited()
{
  if (current_evidence >= local_evidence_list.size())
    return;
}

void Courtroom::on_evidence_image_name_edited()
{
  if (current_evidence >= local_evidence_list.size())
    return;
}

void Courtroom::on_evidence_image_button_clicked()
{
  QDir dir(get_base_path() + "/evidence/");
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
  QStringList bases = Options::getInstance().mountPaths();
  bases.prepend(get_base_path());
  for (const QString &base : bases) {
    QDir baseDir(base);
    if (filename.startsWith(baseDir.absolutePath() + "/")) {
      dir.setPath(baseDir.absolutePath() + "/evidence");
      break;
    }
  }
  filename = dir.relativeFilePath(filename);
  ui_evidence_image_name->setText(filename);
  on_evidence_image_name_edited();
}

void Courtroom::on_evidence_clicked(int p_id)
{
  int f_real_id = p_id + max_evidence_on_page * current_evidence_page;

  if (f_real_id == local_evidence_list.size()) {
    if (current_evidence_global)
      ao_app->send_server_packet(
          new AOPacket("PE", {"<name>", "<description>", "empty.png"}));
    else {
      evi_type f_evi;
      f_evi.name = "<name>";
      f_evi.description = "<description>";
      f_evi.image = "empty.png";

      local_evidence_list.append(f_evi);
      private_evidence_list = local_evidence_list;
      set_evidence_page();
    }
    return;
  }
  else if (f_real_id > local_evidence_list.size())
    return;
  
  if (!Options::getInstance().evidenceDoubleClickEdit()){
    on_evidence_double_clicked(p_id);
    return;
  }

  if (ui_evidence_overlay->isVisible()) {
    return;
  }

  ui_evidence_name->setText(local_evidence_list.at(f_real_id).name);
  for (AOEvidenceButton *i_button : qAsConst(ui_evidence_list))
    i_button->set_selected(false);

  ui_evidence_list.at(p_id)->set_selected(true);
  current_evidence = f_real_id;
}

void Courtroom::on_evidence_double_clicked(int p_id)
{
  int f_real_id = p_id + max_evidence_on_page * current_evidence_page;

  if (f_real_id >= local_evidence_list.size())
    return;

  if (ui_evidence_overlay->isVisible()) {
    if (!on_evidence_x_clicked()) {
      // We're told not to switch over to the other evidence ("cancel" clicked)
      return;
    }
  }

  for (AOEvidenceButton *i_button : qAsConst(ui_evidence_list))
    i_button->set_selected(false);

  // We have to check if the ID is on the currently displayed page.
  // This is because SOMEONE allowed the switching of pages while evidence is still being edited.
  if (p_id < ui_evidence_list.count()) {
    ui_evidence_list.at(p_id)->set_selected(true);
  }
  current_evidence = f_real_id;

  evi_type f_evi = local_evidence_list.at(f_real_id);

  ui_evidence_description->clear();
  ui_evidence_description->appendPlainText(f_evi.description);
  ui_evidence_description->setReadOnly(false);
  ui_evidence_description->setToolTip(tr("Click to edit..."));
  ui_evidence_description->moveCursor(QTextCursor::Start);

  ui_evidence_name->setText(f_evi.name);
  ui_evidence_name->setReadOnly(false);
  ui_evidence_name->setToolTip(tr("Click to edit..."));

  ui_evidence_image_name->setText(f_evi.image);
  ui_evidence_image_name->setReadOnly(false);
  ui_evidence_image_name->setToolTip(tr("Click to edit..."));

  ui_evidence->show();
  ui_evidence_overlay->show();
  ui_evidence_ok->hide();
}

void Courtroom::on_evidence_hover(int p_id, bool p_state)
{
  if (ui_evidence_overlay->isVisible()) {
    // Ignore hovering behavior if we're in the process of viewing a piece of evidence
    return;
  }

  int final_id = p_id + max_evidence_on_page * current_evidence_page;

  if (p_state) {
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
}

void Courtroom::on_evidence_right_clicked()
{
  ++current_evidence_page;

  set_evidence_page();
}

void Courtroom::on_evidence_present_clicked()
{
  if (!current_evidence_global) {
    ui_evidence_present->hide();
    is_presenting_evidence = false;
    return; // otherwise we get force-disconnected
  }
  if (is_presenting_evidence)
    ui_evidence_present->set_image("present");
  else
    ui_evidence_present->set_image("present_disabled");

  is_presenting_evidence = !is_presenting_evidence;

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_delete_clicked()
{
  evidence_close();
  if (current_evidence_global)
    ao_app->send_server_packet(
        new AOPacket("DE", {QString::number(current_evidence)}));
  else {
    local_evidence_list.remove(current_evidence);
    private_evidence_list = local_evidence_list;
    set_evidence_page();

    // Autosave private evidence
    evidence_save("inventories/autosave.ini");
  }

  current_evidence = 0;
}

bool Courtroom::on_evidence_x_clicked()
{
  if (current_evidence >=
      local_evidence_list.size()) // Should never happen but you never know.
    return true;

  if (ui_evidence_ok->isHidden()) {
    // Nothing was modified
    evidence_close();
    return true;
  }
  QMessageBox *msgBox = new QMessageBox;
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setText(tr("Evidence has been modified."));
  msgBox->setInformativeText(tr("Do you want to save your changes?"));
  msgBox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                             QMessageBox::Cancel);
  msgBox->setDefaultButton(QMessageBox::Save);
  int ret = msgBox->exec();
  switch (ret) {
  case QMessageBox::Save:
    evidence_close();
    on_evidence_ok_clicked();
    return true;
  case QMessageBox::Discard:
    evidence_close();
    return true;
  case QMessageBox::Cancel:
  default:
    // Cancel was clicked, report that.
    return false;
  }
}

void Courtroom::on_evidence_ok_clicked()
{
  ui_evidence_ok->hide();
  if (current_evidence < local_evidence_list.size()) {
    evi_type f_evi = local_evidence_list.at(current_evidence);
    if (current_evidence_global) {
      QStringList f_contents;
      f_contents.append(QString::number(current_evidence));
      f_contents.append(ui_evidence_name->text());
      f_contents.append(ui_evidence_description->toPlainText());
      f_contents.append(ui_evidence_image_name->text());

      ao_app->send_server_packet(new AOPacket("EE", f_contents));
    }
    else {
      f_evi.name = ui_evidence_name->text();
      f_evi.description = ui_evidence_description->toPlainText();
      f_evi.image = ui_evidence_image_name->text();
      local_evidence_list.replace(current_evidence, f_evi);
      private_evidence_list = local_evidence_list;
      ui_evidence_ok->hide();
      set_evidence_page();
    }
  }

  // Autosave private evidence
  if (!current_evidence_global) {
    evidence_save("inventories/autosave.ini");
  }
}

void Courtroom::on_evidence_switch_clicked()
{
  evidence_switch(!current_evidence_global);
}

void Courtroom::on_evidence_transfer_clicked()
{
  if (current_evidence >= local_evidence_list.size())
    return;

  QString name;
  if (!current_evidence_global) // Transfer private evidence to global
  {
    evi_type f_evi = local_evidence_list.at(current_evidence);

    QStringList f_contents;
    f_contents.append(f_evi.name);
    f_contents.append(f_evi.description);
    f_contents.append(f_evi.image);

    name = f_evi.name;
    ao_app->send_server_packet(new AOPacket("PE", f_contents));
  }
  else // Transfer global evidence to private
  {
    evi_type f_evi = local_evidence_list.at(current_evidence);
    name = f_evi.name;
    private_evidence_list.append(f_evi);

    // Autosave private evidence
    evidence_save("inventories/autosave.ini");
  }

  QMessageBox *msgBox = new QMessageBox;
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setText(tr("\"%1\" has been transferred.").arg(name));
  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setDefaultButton(QMessageBox::Ok);
  msgBox->exec();
}

void Courtroom::on_evidence_edited()
{
  if (current_evidence >=
      local_evidence_list.size()) // Should never happen but you never know.
    return;
  evi_type fake_evidence;
  fake_evidence.name = ui_evidence_name->text();
  fake_evidence.description = ui_evidence_description->toPlainText();
  fake_evidence.image = ui_evidence_image_name->text();
  if (compare_evidence_changed(fake_evidence,
                               local_evidence_list.at(current_evidence)))
    ui_evidence_ok->show();
  else
    ui_evidence_ok->hide();
}

void Courtroom::evidence_close()
{
  ui_evidence_description->setReadOnly(true);
  ui_evidence_name->setReadOnly(true);
  ui_evidence_image_name->setReadOnly(true);
  ui_evidence_overlay->hide();
}

void Courtroom::evidence_switch(bool global)
{
  evidence_close();
  current_evidence_global = global;
  is_presenting_evidence = false;
  ui_evidence_present->set_image("present");
  local_evidence_list.clear();
  if (current_evidence_global) {
    local_evidence_list = global_evidence_list;
    ui_evidence_present->show();
    ui_evidence_save->hide();
    ui_evidence_load->hide();
    ui_evidence_switch->set_image("evidence_global");
    ui_evidence->set_image("evidence_background");
    ui_evidence_overlay->set_image("evidence_overlay");
    ui_evidence_transfer->set_image("evidence_transfer");
    ui_evidence_transfer->setToolTip(
        tr("Transfer evidence to private inventory."));
    ui_evidence_switch->setToolTip(
        tr("Current evidence is global. Click to switch to private."));
  }
  else {
    local_evidence_list = private_evidence_list;
    ui_evidence_present->hide();
    ui_evidence_save->show();
    ui_evidence_load->show();
    ui_evidence_switch->set_image("evidence_private");
    ui_evidence->set_image("evidence_background_private");
    ui_evidence_overlay->set_image("evidence_overlay_private");
    ui_evidence_transfer->set_image("evidence_transfer_private");
    ui_evidence_transfer->setToolTip(
        tr("Transfer evidence to global inventory."));
    ui_evidence_switch->setToolTip(
        tr("Current evidence is private. Click to switch to global."));
  }
  current_evidence_page = 0;
  set_evidence_page();
}

void Courtroom::on_evidence_save_clicked()
{
  if (current_evidence_global)
    return; // Don't allow saving/loading operations when in global inventory
            // mode for now

  // "Inventories" dir keeps our private evidence data
  if (!dir_exists("inventories")) {
    // Create one if it doesn't yet exist
    QDir("inventories").mkdir("inventories");
  }

  QString p_path = QFileDialog::getSaveFileName(
      this, tr("Save Inventory"), "inventories/", tr("Ini Files (*.ini)"));

  evidence_close();
  ui_evidence_name->setText("");

  evidence_save(p_path);
}

void Courtroom::on_evidence_load_clicked()
{
  if (current_evidence_global)
    return; // Don't allow saving/loading operations when in global inventory
            // mode for now

  QString p_path = QFileDialog::getOpenFileName(
      this, tr("Open Inventory"), "inventories/", tr("Ini Files (*.ini)"));
  if (p_path.isEmpty())
    return;

  evidence_close();
  ui_evidence_name->setText("");
  evidence_load(p_path);
  local_evidence_list = private_evidence_list;
  set_evidence_page();
}

void Courtroom::evidence_load(QString filename)
{
  if (!file_exists(filename)) {
    qWarning() << "Trying to load a non-existant evidence save file:" << filename;
    return;
  }
  QSettings inventory(filename, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  inventory.setIniCodec("UTF-8");
#endif
  private_evidence_list.clear();
  foreach (QString evi, inventory.childGroups()) {
    if (evi == "General")
      continue;

    evi_type f_evi;
    f_evi.name = inventory.value(evi + "/name", "<name>").value<QString>();
    f_evi.description =
        inventory.value(evi + "/description", "<description>").value<QString>();
    f_evi.image =
        inventory.value(evi + "/image", "empty.png").value<QString>();
    private_evidence_list.append(f_evi);
  }
}

void Courtroom::evidence_save(QString filename)
{
  // "Inventories" dir keeps our private evidence data
  if (!dir_exists("inventories")) {
    // Create one if it doesn't yet exist
    QDir("inventories").mkdir("inventories");
  }

  QSettings inventory(filename, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  inventory.setIniCodec("UTF-8");
#endif
  inventory.clear();
  for (int i = 0; i < private_evidence_list.size(); i++) {
    inventory.beginGroup(QString::number(i));
    inventory.setValue("name", private_evidence_list[i].name);
    inventory.setValue("description", private_evidence_list[i].description);
    inventory.setValue("image", private_evidence_list[i].image);
    inventory.endGroup();
  }
  inventory.sync();
}

bool Courtroom::compare_evidence_changed(evi_type evi_a, evi_type evi_b)
{
  return evi_a.name != evi_b.name || evi_a.image != evi_b.image ||
         evi_a.description != evi_b.description;
}
