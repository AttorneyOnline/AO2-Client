#include "courtroom.h"

#include "aoemotebutton.h"

void Courtroom::initialize_emotes()
{
  ui_emotes = new QWidget(this);
  ui_emotes->setObjectName("ui_emotes");

  ui_emote_left = new AOButton(this, ao_app);
  ui_emote_left->setObjectName("ui_emote_left");
  ui_emote_right = new AOButton(this, ao_app);
  ui_emote_right->setObjectName("ui_emote_right");

  ui_emote_dropdown = new QComboBox(this);
  ui_emote_dropdown->setObjectName("ui_emote_dropdown");

  connect(ui_emote_left, &AOButton::clicked, this,
          &Courtroom::on_emote_left_clicked);
  connect(ui_emote_right, &AOButton::clicked, this,
          &Courtroom::on_emote_right_clicked);

  connect(ui_emote_dropdown, QOverload<int>::of(&QComboBox::activated), this,
          &Courtroom::on_emote_dropdown_changed);
}

void Courtroom::refresh_emotes()
{
  // Should properly refresh the emote list
  qDeleteAll(ui_emote_list.begin(), ui_emote_list.end());
  ui_emote_list.clear();

  set_size_and_pos(ui_emotes, "emotes");

  set_size_and_pos(ui_emote_left, "emote_left");
  ui_emote_left->set_image("arrow_left");

  set_size_and_pos(ui_emote_right, "emote_right");
  ui_emote_right->set_image("arrow_right");

  QPoint f_spacing = ao_app->get_button_spacing("emote_button_spacing",
                                                "courtroom_design.ini");
  QPoint p_point =
      ao_app->get_button_spacing("emote_button_size", "courtroom_design.ini");

  if (ui_emotes->width() == 0 || ui_emotes->height() == 0) { // Workaround for a nasty crash
    ui_emotes->hide();
    return;
  }

  const int button_width = p_point.x();
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = p_point.y();
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  emote_columns =
      ((ui_emotes->width() - button_width) / (x_spacing + button_width)) + 1;
  emote_rows =
      ((ui_emotes->height() - button_height) / (y_spacing + button_height)) + 1;

  max_emotes_on_page = emote_columns * emote_rows;

  QString selected_image = ao_app->get_image_suffix(ao_app->get_theme_path("emote_selected", ""), true);

  for (int n = 0; n < max_emotes_on_page; ++n) {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEmoteButton *f_emote = new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos,
                                               button_width, button_height);
    f_emote->set_selected_image(selected_image);
    ui_emote_list.append(f_emote);

    f_emote->set_id(n);

    connect(f_emote, &AOEmoteButton::emote_clicked, this,
            &Courtroom::on_emote_clicked);

    ++x_mod_count;

    if (x_mod_count == emote_columns) {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }
}

void Courtroom::set_emote_page()
{
  if (m_cid == -1)
    return;

  int total_emotes = ao_app->get_emote_number(current_char);

  ui_emote_left->hide();
  ui_emote_right->hide();

  for (AOEmoteButton *i_button : qAsConst(ui_emote_list)) {
    i_button->hide();
  }

  int total_pages = total_emotes / max_emotes_on_page;
  int emotes_on_page = 0;

  if (total_emotes % max_emotes_on_page != 0) {
    ++total_pages;
    // i. e. not on the last page
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

  for (int n_emote = 0;
       n_emote < emotes_on_page && n_emote < ui_emote_list.size(); ++n_emote) {
    int n_real_emote = n_emote + current_emote_page * max_emotes_on_page;
    AOEmoteButton *f_emote = ui_emote_list.at(n_emote);

    if (n_real_emote == current_emote)
      f_emote->set_char_image(current_char, n_real_emote, true);
    else
      f_emote->set_char_image(current_char, n_real_emote, false);

    f_emote->show();
    f_emote->setToolTip(QString::number(n_real_emote + 1) + ": " +
                        ao_app->get_emote_comment(current_char, n_real_emote));
  }
}

void Courtroom::set_emote_dropdown()
{
  ui_emote_dropdown->clear();

  int total_emotes = ao_app->get_emote_number(current_char);

  for (int n = 0; n < total_emotes; ++n) {
    ui_emote_dropdown->addItem(QString::number(n + 1) + ": " +
                               ao_app->get_emote_comment(current_char, n));
    QString icon_path = ao_app->get_image_suffix(ao_app->get_character_path(
                                                   current_char, "emotions/button" + QString::number(n + 1) + "_off"));
    ui_emote_dropdown->setItemIcon(n, QIcon(icon_path));
  }
}

void Courtroom::select_emote(int p_id)
{
  int min = current_emote_page * max_emotes_on_page;
  int max = (max_emotes_on_page - 1) + current_emote_page * max_emotes_on_page;

  if (current_emote >= min && current_emote <= max)
    ui_emote_list.at(current_emote % max_emotes_on_page)
        ->set_char_image(current_char, current_emote, false);

  int old_emote = current_emote;

  current_emote = p_id;

  if (current_emote >= min && current_emote <= max)
    ui_emote_list.at(current_emote % max_emotes_on_page)
        ->set_char_image(current_char, current_emote, true);

  int emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  if (old_emote == current_emote) {
    ui_pre->setChecked(!ui_pre->isChecked());
  }
  else if (!ao_app->is_stickypres_enabled()) {
    if (emote_mod == PREANIM || emote_mod == PREANIM_ZOOM) {
      ui_pre->setChecked(true);
    }
    else {
      ui_pre->setChecked(false);
    }
  }

  ui_emote_dropdown->setCurrentIndex(current_emote);

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_clicked(int p_id)
{
  select_emote(p_id + max_emotes_on_page * current_emote_page);
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
  select_emote(p_index);
}
