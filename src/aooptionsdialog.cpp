#include "aooptionsdialog.h"
#include "aoapplication.h"
#include "courtroom.h"
#include "lobby.h"
#include "bass.h"
#include "networkmanager.h"

#include <QFileDialog>

AOOptionsDialog::AOOptionsDialog(QWidget *parent, AOApplication *p_ao_app)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
  ao_app = p_ao_app;

  // Setting up the basics.
  setWindowFlag(Qt::WindowCloseButtonHint);
  setWindowTitle(tr("Settings"));
  resize(450, 408);

  ui_settings_buttons = new QDialogButtonBox(this);

  QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(
      ui_settings_buttons->sizePolicy().hasHeightForWidth());
  ui_settings_buttons->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
  ui_settings_buttons->setOrientation(Qt::Horizontal);
  ui_settings_buttons->setStandardButtons(QDialogButtonBox::Cancel |
                                          QDialogButtonBox::Save |
                                          QDialogButtonBox::RestoreDefaults |
                                          QDialogButtonBox::Help);
  ui_settings_buttons->button(QDialogButtonBox::Help)->setText(tr("About"));

  connect(ui_settings_buttons, &QDialogButtonBox::accepted, this,
                   &AOOptionsDialog::save_pressed);
  connect(ui_settings_buttons, &QDialogButtonBox::rejected, this,
                   &AOOptionsDialog::discard_pressed);
  connect(ui_settings_buttons, &QDialogButtonBox::clicked, this,
                   &AOOptionsDialog::button_clicked);
  connect(ui_settings_buttons, &QDialogButtonBox::helpRequested, this,
                   &AOOptionsDialog::about_button_pressed);

  // We'll stop updates so that the window won't flicker while it's being made.
  setUpdatesEnabled(false);

  // First of all, we want a tabbed dialog, so let's add some layout.
  ui_vertical_layout = new QVBoxLayout(this);
  ui_settings_tabs = new QTabWidget(this);

  ui_vertical_layout->addWidget(ui_settings_tabs);
  ui_vertical_layout->addWidget(ui_settings_buttons);

  // Let's add the tabs one by one.
  // First, we'll start with 'Gameplay'.
  ui_gameplay_tab = new QWidget(this);
  ui_gameplay_tab->setSizePolicy(sizePolicy1);
  ui_settings_tabs->addTab(ui_gameplay_tab, tr("Gameplay"));
  ui_form_layout_widget = new QWidget(ui_gameplay_tab);
  ui_form_layout_widget->setSizePolicy(sizePolicy1);

  ui_gameplay_form = new QFormLayout(ui_form_layout_widget);
  ui_gameplay_form->setLabelAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                      Qt::AlignVCenter);
  ui_gameplay_form->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                     Qt::AlignTop);
  ui_gameplay_form->setContentsMargins(0, 0, 0, 0);
  ui_gameplay_form->setSpacing(4);

  int row = 0;

  ui_theme_label = new QLabel(ui_form_layout_widget);
  ui_theme_label->setText(tr("Theme:"));
  ui_theme_label->setToolTip(
      tr("Sets the theme used in-game. If the new theme changes "
         "the lobby's look as well, you'll need to reload the "
         "lobby for the changes to take effect, such as by joining "
         "a server and leaving it."));
  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_theme_label);
  ui_theme_combobox = new QComboBox(ui_form_layout_widget);

  // Fill the combobox with the names of the themes.
  QSet<QString> themes;
  QStringList bases = ao_app->get_mount_paths();
  bases.push_front(ao_app->get_base_path());
  for (const QString &base : bases) {
    QDirIterator it(base + "/themes", QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
      QString actualname = QDir(it.next()).dirName();
      if (!themes.contains(actualname)) {
        ui_theme_combobox->addItem(actualname);
        themes.insert(actualname);
      }
    }
  }

  connect(ui_theme_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                   &AOOptionsDialog::theme_changed);
  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_theme_combobox);

  row += 1;

  ui_subtheme_label = new QLabel(ui_form_layout_widget);
  ui_subtheme_label->setText(tr("Subtheme:"));
  ui_subtheme_label->setToolTip(
      tr("Sets a 'subtheme', which will stack on top of the current theme and replace anything it can."
         "Keep it at 'server' to let the server decide. Keep it at 'default' to keep it unchanging."));
  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_subtheme_label);
  ui_subtheme_combobox = new QComboBox(ui_form_layout_widget);

  // Fill the combobox with the names of the themes.
  ui_subtheme_combobox->addItem("server");
  ui_subtheme_combobox->addItem("default");
  QDirIterator it2(ao_app->get_real_path(ao_app->get_theme_path("")), QDir::Dirs,
                  QDirIterator::NoIteratorFlags);
  while (it2.hasNext()) {
    QString actualname = QDir(it2.next()).dirName();
    if (actualname != "." && actualname != ".." && actualname.toLower() != "server" && actualname.toLower() != "default" && actualname.toLower() != "effects" && actualname.toLower() != "misc") {
      ui_subtheme_combobox->addItem(actualname);
    }
  }

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_subtheme_combobox);

  row += 1;

  ui_theme_reload_button = new QPushButton(ui_form_layout_widget);
  ui_theme_reload_button->setText(tr("Reload Theme"));
  ui_theme_reload_button->setToolTip(
      tr("Refresh the theme and update all of the ui elements to match."));
  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_theme_reload_button);
  connect(ui_theme_reload_button, &QPushButton::clicked, this,
          &AOOptionsDialog::on_reload_theme_clicked);

  row += 1;
  ui_animated_theme_lbl = new QLabel(ui_form_layout_widget);
  ui_animated_theme_lbl->setText(tr("Animated Theme:"));
  ui_animated_theme_lbl->setToolTip(
      tr("If ticked, themes will be allowed to have animated elements."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_animated_theme_lbl);

  ui_animated_theme_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_animated_theme_cb);

  row += 1;
  ui_theme_log_divider = new QFrame(ui_form_layout_widget);
  ui_theme_log_divider->setMidLineWidth(0);
  ui_theme_log_divider->setFrameShape(QFrame::HLine);
  ui_theme_log_divider->setFrameShadow(QFrame::Sunken);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole,
                              ui_theme_log_divider);
  
  row += 1;
  ui_stay_time_lbl = new QLabel(ui_form_layout_widget);
  ui_stay_time_lbl->setText(tr("Text Stay Time:"));
  ui_stay_time_lbl->setToolTip(tr(
      "Minimum amount of time (in miliseconds) an IC message must stay on screen before "
      "the next IC message is shown, acting as a 'queue'. Set to 0 to disable this behavior."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_stay_time_lbl);

  ui_stay_time_spinbox = new QSpinBox(ui_form_layout_widget);
  ui_stay_time_spinbox->setSuffix(" ms");
  ui_stay_time_spinbox->setMaximum(10000);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_stay_time_spinbox);

  row += 1;
  ui_instant_objection_lbl = new QLabel(ui_form_layout_widget);
  ui_instant_objection_lbl->setText(tr("Instant Objection:"));
  ui_instant_objection_lbl->setToolTip(
      tr("If Text Stay Time is more than 0, instant objection will skip queued messages instead of waiting to catch up."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_instant_objection_lbl);

  ui_instant_objection_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_instant_objection_cb);

  row += 1;
  ui_text_crawl_lbl = new QLabel(ui_form_layout_widget);
  ui_text_crawl_lbl->setText(tr("Text crawl:"));
  ui_text_crawl_lbl->setToolTip(tr(
      "Amount of time (in miliseconds) spent on each letter when the in-character text is being displayed."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_text_crawl_lbl);

  ui_text_crawl_spinbox = new QSpinBox(ui_form_layout_widget);
  ui_text_crawl_spinbox->setSuffix(" ms");
  ui_text_crawl_spinbox->setMaximum(500);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_text_crawl_spinbox);

  row += 1;
  ui_chat_ratelimit_lbl = new QLabel(ui_form_layout_widget);
  ui_chat_ratelimit_lbl->setText(tr("Chat Rate Limit:"));
  ui_chat_ratelimit_lbl->setToolTip(tr(
      "Minimum amount of time (in miliseconds) that must pass before the next Enter key press will send your IC message."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_chat_ratelimit_lbl);

  ui_chat_ratelimit_spinbox = new QSpinBox(ui_form_layout_widget);
  ui_chat_ratelimit_spinbox->setSuffix(" ms");
  ui_chat_ratelimit_spinbox->setMaximum(5000);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_chat_ratelimit_spinbox);

  row += 1;
  ui_log_names_divider = new QFrame(ui_form_layout_widget);
  ui_log_names_divider->setFrameShape(QFrame::HLine);
  ui_log_names_divider->setFrameShadow(QFrame::Sunken);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole,
                              ui_log_names_divider);

  row += 1;
  ui_username_lbl = new QLabel(ui_form_layout_widget);
  ui_username_lbl->setText(tr("Default username:"));
  ui_username_lbl->setToolTip(
      tr("Your OOC name will be automatically set to this value "
         "when you join a server."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_username_lbl);

  ui_username_textbox = new QLineEdit(ui_form_layout_widget);
  ui_username_textbox->setMaxLength(30);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_username_textbox);

  row += 1;
  ui_showname_lbl = new QLabel(ui_form_layout_widget);
  ui_showname_lbl->setText(tr("Custom shownames:"));
  ui_showname_lbl->setToolTip(
      tr("Gives the default value for the in-game 'Custom shownames' "
         "tickbox, which in turn determines whether the client should "
         "display custom in-character names."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_showname_lbl);

  ui_showname_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_showname_cb);

  row +=1;
  ui_default_showname_lbl = new QLabel(ui_form_layout_widget);
  ui_default_showname_lbl->setText(tr("Default showname:"));
  ui_default_showname_lbl->setToolTip(
              tr("Your showname will be automatically set to this value "
                 "when you join a server."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_default_showname_lbl);

  ui_default_showname_textbox = new QLineEdit(ui_form_layout_widget);
  ui_default_showname_textbox->setMaxLength(30);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_default_showname_textbox);

  row += 1;
  ui_net_divider = new QFrame(ui_form_layout_widget);
  ui_net_divider->setFrameShape(QFrame::HLine);
  ui_net_divider->setFrameShadow(QFrame::Sunken);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_net_divider);

  row += 1;
  ui_ms_lbl = new QLabel(ui_form_layout_widget);
  ui_ms_lbl->setText(tr("Alternate Server List:"));
  ui_ms_lbl->setToolTip(
      tr("Overrides the base URL to retrieve server information from."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_ms_lbl);

  ui_ms_textbox = new QLineEdit(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_ms_textbox);

  row += 1;
  ui_discord_lbl = new QLabel(ui_form_layout_widget);
  ui_discord_lbl->setText(tr("Discord:"));
  ui_discord_lbl->setToolTip(
      tr("Allows others on Discord to see what server you are in, "
         "what character are you playing, and how long you have "
         "been playing for."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_discord_lbl);

  ui_discord_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_discord_cb);

  row += 1;
  ui_language_label = new QLabel(ui_form_layout_widget);
  ui_language_label->setText(tr("Language:"));
  ui_language_label->setToolTip(
      tr("Sets the language if you don't want to use your system language."));
  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_language_label);

  ui_language_combobox = new QComboBox(ui_form_layout_widget);
  ui_language_combobox->addItem(
      ao_app->configini->value("language", "  ").value<QString>() +
      tr(" - Keep current setting"));
  ui_language_combobox->addItem("   - Default");
  ui_language_combobox->addItem("en - English");
  ui_language_combobox->addItem("de - Deutsch");
  ui_language_combobox->addItem("es - Español");
  ui_language_combobox->addItem("pt - Português");
  ui_language_combobox->addItem("pl - Polskie");
  ui_language_combobox->addItem("jp - 日本語");
  ui_language_combobox->addItem("ru - Русский");
  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole,
                              ui_language_combobox);

  row += 1;
  ui_scaling_label = new QLabel(ui_form_layout_widget);
  ui_scaling_label->setText(tr("Scaling:"));
  ui_scaling_label->setToolTip(
        tr("Sets the default scaling method, if there is not one already defined "
           "specifically for the character."));
  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_scaling_label);

  ui_scaling_combobox = new QComboBox(ui_form_layout_widget);
  // Corresponds with Qt::TransformationMode enum. Please don't change the order.
  ui_scaling_combobox->addItem(tr("Pixel"), "fast");
  ui_scaling_combobox->addItem(tr("Smooth"), "smooth");
  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_scaling_combobox);

  row += 1;
  ui_shake_lbl = new QLabel(ui_form_layout_widget);
  ui_shake_lbl->setText(tr("Allow Screenshake:"));
  ui_shake_lbl->setToolTip(
      tr("Allows screenshaking. Disable this if you have concerns or issues "
         "with photosensitivity and/or seizures."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_shake_lbl);

  ui_shake_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_shake_cb);

  row += 1;
  ui_effects_lbl = new QLabel(ui_form_layout_widget);
  ui_effects_lbl->setText(tr("Allow Effects:"));
  ui_effects_lbl->setToolTip(
      tr("Allows screen effects. Disable this if you have concerns or issues "
         "with photosensitivity and/or seizures."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_effects_lbl);

  ui_effects_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_effects_cb);

  row += 1;
  ui_framenetwork_lbl = new QLabel(ui_form_layout_widget);
  ui_framenetwork_lbl->setText(tr("Network Frame Effects:"));
  ui_framenetwork_lbl->setToolTip(tr(
      "Send screen-shaking, flashes and sounds as defined in the char.ini over "
      "the network. Only works for servers that support this functionality."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_framenetwork_lbl);

  ui_framenetwork_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_framenetwork_cb);

  row += 1;
  ui_colorlog_lbl = new QLabel(ui_form_layout_widget);
  ui_colorlog_lbl->setText(tr("Colors in IC Log:"));
  ui_colorlog_lbl->setToolTip(
      tr("Use the markup colors in the server IC chatlog."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_colorlog_lbl);

  ui_colorlog_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_colorlog_cb);

  row += 1;
  ui_stickysounds_lbl = new QLabel(ui_form_layout_widget);
  ui_stickysounds_lbl->setText(tr("Sticky Sounds:"));
  ui_stickysounds_lbl->setToolTip(
      tr("Turn this on to prevent the sound dropdown from clearing the sound "
         "after playing it."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_stickysounds_lbl);

  ui_stickysounds_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_stickysounds_cb);

  row += 1;
  ui_stickyeffects_lbl = new QLabel(ui_form_layout_widget);
  ui_stickyeffects_lbl->setText(tr("Sticky Effects:"));
  ui_stickyeffects_lbl->setToolTip(
      tr("Turn this on to prevent the effects dropdown from clearing the "
         "effect after playing it."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole,
                              ui_stickyeffects_lbl);

  ui_stickyeffects_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_stickyeffects_cb);

  row += 1;
  ui_stickypres_lbl = new QLabel(ui_form_layout_widget);
  ui_stickypres_lbl->setText(tr("Sticky Preanims:"));
  ui_stickypres_lbl->setToolTip(
      tr("Turn this on to prevent preanimation checkbox from clearing after "
         "playing the emote."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_stickypres_lbl);

  ui_stickypres_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_stickypres_cb);

  row += 1;
  ui_customchat_lbl = new QLabel(ui_form_layout_widget);
  ui_customchat_lbl->setText(tr("Custom Chatboxes:"));
  ui_customchat_lbl->setToolTip(
      tr("Turn this on to allow characters to define their own "
         "custom chat box designs."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_customchat_lbl);

  ui_customchat_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_customchat_cb);

  row += 1;
  ui_sticker_lbl = new QLabel(ui_form_layout_widget);
  ui_sticker_lbl->setText(tr("Stickers:"));
  ui_sticker_lbl->setToolTip(
      tr("Turn this on to allow characters to define their own "
         "stickers (unique images that show up over the chatbox - like avatars or shownames)."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_sticker_lbl);

  ui_sticker_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_sticker_cb);

  row += 1;
  ui_continuous_lbl = new QLabel(ui_form_layout_widget);
  ui_continuous_lbl->setText(tr("Continuous Playback:"));
  ui_continuous_lbl->setToolTip(
      tr("Whether or not to resume playing animations from where they left off. Turning off might reduce lag."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_continuous_lbl);

  ui_continuous_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_continuous_cb);

  row += 1;
  ui_category_stop_lbl = new QLabel(ui_form_layout_widget);
  ui_category_stop_lbl->setText(tr("Stop Music w/ Category:"));
  ui_category_stop_lbl->setToolTip(
      tr("Stop music when double-clicking a category. If this is disabled, use the right-click context menu to stop music."));

  ui_gameplay_form->setWidget(row, QFormLayout::LabelRole, ui_category_stop_lbl);

  ui_category_stop_cb = new QCheckBox(ui_form_layout_widget);

  ui_gameplay_form->setWidget(row, QFormLayout::FieldRole, ui_category_stop_cb);

  // Finish gameplay tab
  QScrollArea *scroll = new QScrollArea(this);
  scroll->setWidget(ui_form_layout_widget);
  ui_gameplay_tab->setLayout(new QVBoxLayout);
  ui_gameplay_tab->layout()->addWidget(scroll);
  ui_gameplay_tab->show();

  // Here we start the callwords tab.
  ui_callwords_tab = new QWidget(this);
  ui_settings_tabs->addTab(ui_callwords_tab, tr("Callwords"));

  ui_callwords_widget = new QWidget(ui_callwords_tab);
  ui_callwords_widget->setGeometry(QRect(10, 10, 361, 211));

  ui_callwords_layout = new QVBoxLayout(ui_callwords_widget);
  ui_callwords_layout->setContentsMargins(0, 0, 0, 0);

  ui_callwords_textbox = new QPlainTextEdit(ui_callwords_widget);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(
      ui_callwords_textbox->sizePolicy().hasHeightForWidth());
  ui_callwords_textbox->setSizePolicy(sizePolicy);

  ui_callwords_layout->addWidget(ui_callwords_textbox);

  ui_callwords_explain_lbl = new QLabel(ui_callwords_widget);
  ui_callwords_explain_lbl->setWordWrap(true);
  ui_callwords_explain_lbl->setText(
      tr("<html><head/><body>Enter as many callwords as you would like. These "
         "are case insensitive. Make sure to leave every callword in its own "
         "line!<br>Do not leave a line with a space at the end -- you will be "
         "alerted everytime someone uses a space in their "
         "messages.</body></html>"));

  ui_callwords_layout->addWidget(ui_callwords_explain_lbl);

  // The audio tab.
  ui_audio_tab = new QWidget(this);
  ui_settings_tabs->addTab(ui_audio_tab, tr("Audio"));

  ui_audio_widget = new QWidget(ui_audio_tab);
  ui_audio_widget->setGeometry(QRect(10, 10, 361, 211));

  ui_audio_layout = new QFormLayout(ui_audio_widget);
  ui_audio_layout->setLabelAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                     Qt::AlignVCenter);
  ui_audio_layout->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                    Qt::AlignTop);
  ui_audio_layout->setContentsMargins(0, 0, 0, 0);
  row = 0;

  ui_audio_device_lbl = new QLabel(ui_audio_widget);
  ui_audio_device_lbl->setText(tr("Audio device:"));
  ui_audio_device_lbl->setToolTip(tr("Sets the audio device for all sounds."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_audio_device_lbl);

  ui_audio_device_combobox = new QComboBox(ui_audio_widget);

  // Let's fill out the combobox with the available audio devices. Or don't if
  // there is no audio
  int a = 0;
  if (needs_default_audiodev()) {

    ui_audio_device_combobox->addItem("default"); //TODO translate this without breaking the default audio device
  }
  BASS_DEVICEINFO info;
  for (a = 0; BASS_GetDeviceInfo(a, &info); a++) {
    ui_audio_device_combobox->addItem(info.name);
    if (ao_app->get_audio_output_device() == info.name)
      ui_audio_device_combobox->setCurrentIndex(
          ui_audio_device_combobox->count() - 1);
  }
  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_audio_device_combobox);

  row += 1;
  ui_audio_volume_divider = new QFrame(ui_audio_widget);
  ui_audio_volume_divider->setFrameShape(QFrame::HLine);
  ui_audio_volume_divider->setFrameShadow(QFrame::Sunken);

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_audio_volume_divider);

  row += 1;
  ui_music_volume_lbl = new QLabel(ui_audio_widget);
  ui_music_volume_lbl->setText(tr("Music:"));
  ui_music_volume_lbl->setToolTip(tr("Sets the music's default volume."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_music_volume_lbl);

  ui_music_volume_spinbox = new QSpinBox(ui_audio_widget);
  ui_music_volume_spinbox->setMaximum(100);
  ui_music_volume_spinbox->setSuffix("%");

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_music_volume_spinbox);

  row += 1;
  ui_sfx_volume_lbl = new QLabel(ui_audio_widget);
  ui_sfx_volume_lbl->setText(tr("SFX:"));
  ui_sfx_volume_lbl->setToolTip(
      tr("Sets the SFX's default volume. "
         "Interjections and actual sound effects count as 'SFX'."));
  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_sfx_volume_lbl);

  ui_sfx_volume_spinbox = new QSpinBox(ui_audio_widget);
  ui_sfx_volume_spinbox->setMaximum(100);
  ui_sfx_volume_spinbox->setSuffix("%");

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_sfx_volume_spinbox);

  row += 1;
  ui_blips_volume_lbl = new QLabel(ui_audio_widget);
  ui_blips_volume_lbl->setText(tr("Blips:"));
  ui_blips_volume_lbl->setToolTip(
      tr("Sets the volume of the blips, the talking sound effects."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_blips_volume_lbl);

  ui_blips_volume_spinbox = new QSpinBox(ui_audio_widget);
  ui_blips_volume_spinbox->setMaximum(100);
  ui_blips_volume_spinbox->setSuffix("%");

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_blips_volume_spinbox);

  row += 1;
  ui_volume_blip_divider = new QFrame(ui_audio_widget);
  ui_volume_blip_divider->setFrameShape(QFrame::HLine);
  ui_volume_blip_divider->setFrameShadow(QFrame::Sunken);

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole,
                             ui_volume_blip_divider);

  row += 1;
  ui_bliprate_lbl = new QLabel(ui_audio_widget);
  ui_bliprate_lbl->setText(tr("Blip rate:"));
  ui_bliprate_lbl->setToolTip(
      tr("Sets the delay between playing the blip sounds."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_bliprate_lbl);

  ui_bliprate_spinbox = new QSpinBox(ui_audio_widget);
  ui_bliprate_spinbox->setMinimum(0);
  ui_bliprate_spinbox->setToolTip(
      tr("Play a blip sound \"once per every X symbols\", where "
         "X is the blip rate. 0 plays a blip sound only once."));

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole, ui_bliprate_spinbox);

  row += 1;
  ui_blank_blips_lbl = new QLabel(ui_audio_widget);
  ui_blank_blips_lbl->setText(tr("Blank blips:"));
  ui_blank_blips_lbl->setToolTip(
      tr("If true, the game will play a blip sound even "
         "when a space is 'being said'."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_blank_blips_lbl);

  ui_blank_blips_cb = new QCheckBox(ui_audio_widget);

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole, ui_blank_blips_cb);

  row += 1;
  ui_loopsfx_lbl = new QLabel(ui_audio_widget);
  ui_loopsfx_lbl->setText(tr("Enable Looping SFX:"));
  ui_loopsfx_lbl->setToolTip(tr("If true, the game will allow looping sound "
                                "effects to play on preanimations."));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_loopsfx_lbl);

  ui_loopsfx_cb = new QCheckBox(ui_audio_widget);

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole, ui_loopsfx_cb);

  row += 1;
  ui_objectmusic_lbl = new QLabel(ui_audio_widget);
  ui_objectmusic_lbl->setText(tr("Kill Music On Objection:"));
  ui_objectmusic_lbl->setToolTip(
      tr("If true, kenji will ask the server to stop music when you use 'Objection!' "));

  ui_audio_layout->setWidget(row, QFormLayout::LabelRole, ui_objectmusic_lbl);

  ui_objectmusic_cb = new QCheckBox(ui_audio_widget);

  ui_audio_layout->setWidget(row, QFormLayout::FieldRole, ui_objectmusic_cb);

  // The casing tab!
  ui_casing_tab = new QWidget(this);
  ui_settings_tabs->addTab(ui_casing_tab, tr("Casing"));

  ui_casing_widget = new QWidget(ui_casing_tab);
  ui_casing_widget->setGeometry(QRect(10, 10, 361, 211));

  ui_casing_layout = new QFormLayout(ui_casing_widget);
  ui_casing_layout->setLabelAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                      Qt::AlignVCenter);
  ui_casing_layout->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                     Qt::AlignTop);
  ui_casing_layout->setContentsMargins(0, 0, 0, 0);
  row = 0;

  // -- CASE ANNOUNCEMENTS

  row += 1;
  ui_casing_enabled_lbl = new QLabel(ui_casing_widget);
  ui_casing_enabled_lbl->setText(tr("Casing:"));
  ui_casing_enabled_lbl->setToolTip(
      tr("If checked, you will get alerts about case "
         "announcements."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole,
                              ui_casing_enabled_lbl);

  ui_casing_enabled_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole,
                              ui_casing_enabled_cb);

  // -- DEFENSE ANNOUNCEMENTS

  row += 1;
  ui_casing_def_lbl = new QLabel(ui_casing_widget);
  ui_casing_def_lbl->setText(tr("Defense:"));
  ui_casing_def_lbl->setToolTip(tr("If checked, you will get alerts about case "
                                   "announcements if a defense spot is open."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_def_lbl);

  ui_casing_def_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_def_cb);

  // -- PROSECUTOR ANNOUNCEMENTS

  row += 1;
  ui_casing_pro_lbl = new QLabel(ui_casing_widget);
  ui_casing_pro_lbl->setText(tr("Prosecution:"));
  ui_casing_pro_lbl->setToolTip(
      tr("If checked, you will get alerts about case "
         "announcements if a prosecutor spot is open."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_pro_lbl);

  ui_casing_pro_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_pro_cb);

  // -- JUDGE ANNOUNCEMENTS

  row += 1;
  ui_casing_jud_lbl = new QLabel(ui_casing_widget);
  ui_casing_jud_lbl->setText(tr("Judge:"));
  ui_casing_jud_lbl->setToolTip(tr("If checked, you will get alerts about case "
                                   "announcements if the judge spot is open."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_jud_lbl);

  ui_casing_jud_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_jud_cb);

  // -- JUROR ANNOUNCEMENTS

  row += 1;
  ui_casing_jur_lbl = new QLabel(ui_casing_widget);
  ui_casing_jur_lbl->setText(tr("Juror:"));
  ui_casing_jur_lbl->setToolTip(tr("If checked, you will get alerts about case "
                                   "announcements if a juror spot is open."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_jur_lbl);

  ui_casing_jur_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_jur_cb);

  // -- STENO ANNOUNCEMENTS

  row += 1;
  ui_casing_steno_lbl = new QLabel(ui_casing_widget);
  ui_casing_steno_lbl->setText(tr("Stenographer:"));
  ui_casing_steno_lbl->setToolTip(
      tr("If checked, you will get alerts about case "
         "announcements if a stenographer spot is open."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_steno_lbl);

  ui_casing_steno_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_steno_cb);

  // -- CM ANNOUNCEMENTS

  row += 1;
  ui_casing_cm_lbl = new QLabel(ui_casing_widget);
  ui_casing_cm_lbl->setText(tr("CM:"));
  ui_casing_cm_lbl->setToolTip(
      tr("If checked, you will appear amongst the potential "
         "CMs on the server."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole, ui_casing_cm_lbl);

  ui_casing_cm_cb = new QCheckBox(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole, ui_casing_cm_cb);

  // -- CM CASES ANNOUNCEMENTS

  row += 1;
  ui_casing_cm_cases_lbl = new QLabel(ui_casing_widget);
  ui_casing_cm_cases_lbl->setText(tr("Hosting cases:"));
  ui_casing_cm_cases_lbl->setToolTip(
      tr("If you're a CM, enter what cases you are "
         "willing to host."));

  ui_casing_layout->setWidget(row, QFormLayout::LabelRole,
                              ui_casing_cm_cases_lbl);

  ui_casing_cm_cases_textbox = new QLineEdit(ui_casing_widget);

  ui_casing_layout->setWidget(row, QFormLayout::FieldRole,
                              ui_casing_cm_cases_textbox);

  // Assets tab
  ui_assets_tab = new QWidget(this);
  ui_assets_tab_layout = new QVBoxLayout(ui_assets_tab);
  ui_assets_tab->setLayout(ui_assets_tab_layout);
  ui_settings_tabs->addTab(ui_assets_tab, tr("Assets"));

  ui_asset_lbl = new QLabel(ui_assets_tab);
  ui_asset_lbl->setText(
        tr("Add or remove base folders for use by assets. "
           "Base folders on the bottom are prioritized over those above them."));
  ui_asset_lbl->setWordWrap(true);
  ui_assets_tab_layout->addWidget(ui_asset_lbl);

  ui_mount_list = new QListWidget(ui_assets_tab);
  ui_assets_tab_layout->addWidget(ui_mount_list);

  ui_mount_buttons_layout = new QGridLayout(ui_assets_tab);
  ui_assets_tab_layout->addLayout(ui_mount_buttons_layout);

  QSizePolicy stretch_btns(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  stretch_btns.setHorizontalStretch(4);

  ui_mount_add = new QPushButton(tr("Add…"), ui_assets_tab);
  ui_mount_add->setSizePolicy(stretch_btns);
  ui_mount_buttons_layout->addWidget(ui_mount_add, 0, 0, 1, 1);
  connect(ui_mount_add, &QPushButton::clicked, this, [this] {
    QString path = QFileDialog::getExistingDirectory(this, tr("Select a base folder"),
                                                    QApplication::applicationDirPath(),
                                                    QFileDialog::ShowDirsOnly);
    if (path.isEmpty()) {
      return;
    }
    QDir dir(QApplication::applicationDirPath());
    QString relative = dir.relativeFilePath(path);
    if (!relative.contains("../")) {
      path = relative;
    }
    QListWidgetItem *dir_item = new QListWidgetItem(path);
    ui_mount_list->addItem(dir_item);
    ui_mount_list->setCurrentItem(dir_item);

    // quick hack to update buttons
    emit ui_mount_list->itemSelectionChanged();
  });

  ui_mount_remove = new QPushButton(tr("Remove"), ui_assets_tab);
  ui_mount_remove->setSizePolicy(stretch_btns);
  ui_mount_remove->setEnabled(false);
  ui_mount_buttons_layout->addWidget(ui_mount_remove, 0, 1, 1, 1);
  connect(ui_mount_remove, &QPushButton::clicked, this, [=] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
      return;
    delete selected[0];
    emit ui_mount_list->itemSelectionChanged();
    asset_cache_dirty = true;
  });

  auto *mount_buttons_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                               QSizePolicy::Minimum);
  ui_mount_buttons_layout->addItem(mount_buttons_spacer, 0, 2, 1, 1);

  ui_mount_up = new QPushButton(tr("↑"), ui_assets_tab);
  ui_mount_up->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  ui_mount_up->setMaximumWidth(40);
  ui_mount_up->setEnabled(false);
  ui_mount_buttons_layout->addWidget(ui_mount_up, 0, 3, 1, 1);
  connect(ui_mount_up, &QPushButton::clicked, this, [=] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
      return;
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    ui_mount_list->takeItem(row);
    int new_row = qMax(1, row - 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  ui_mount_down = new QPushButton(tr("↓"), ui_assets_tab);
  ui_mount_down->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  ui_mount_down->setMaximumWidth(40);
  ui_mount_down->setEnabled(false);
  ui_mount_buttons_layout->addWidget(ui_mount_down, 0, 4, 1, 1);
  connect(ui_mount_down, &QPushButton::clicked, this, [=] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
      return;
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    ui_mount_list->takeItem(row);
    int new_row = qMin(ui_mount_list->count() + 1, row + 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  auto *mount_buttons_spacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                                 QSizePolicy::Minimum);
  ui_mount_buttons_layout->addItem(mount_buttons_spacer_2, 0, 5, 1, 1);

  ui_mount_clear_cache = new QPushButton(tr("Clear Cache"), ui_assets_tab);
  ui_mount_clear_cache->setToolTip(tr("Clears the lookup cache for assets. "
  "Use this when you have added an asset that takes precedence over another "
  "existing asset."));
  ui_mount_buttons_layout->addWidget(ui_mount_clear_cache, 0, 6, 1, 1);
  connect(ui_mount_clear_cache, &QPushButton::clicked, this, [=] {
    asset_cache_dirty = true;
    ui_mount_clear_cache->setEnabled(false);
  });

  connect(ui_mount_list, &QListWidget::itemSelectionChanged, this, [=] {
    auto selected_items = ui_mount_list->selectedItems();
    bool row_selected = !ui_mount_list->selectedItems().isEmpty();
    ui_mount_remove->setEnabled(row_selected);
    ui_mount_up->setEnabled(row_selected);
    ui_mount_down->setEnabled(row_selected);

    if (!row_selected)
      return;

    int row = ui_mount_list->row(selected_items[0]);
    if (row <= 1)
      ui_mount_up->setEnabled(false);
    if (row >= ui_mount_list->count() - 1)
      ui_mount_down->setEnabled(false);
  });

  // Logging tab
  ui_logging_tab = new QWidget(this);
  ui_settings_tabs->addTab(ui_logging_tab, tr("Logging"));
  ui_form_logging_widget = new QWidget(this);

  ui_logging_form = new QFormLayout(ui_form_logging_widget);
  ui_logging_form->setLabelAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                     Qt::AlignVCenter);
  ui_logging_form->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                    Qt::AlignTop);
  ui_logging_form->setContentsMargins(5, 5, 0, 0);
  ui_logging_form->setSpacing(4);
  row = 0;

  ui_downwards_lbl = new QLabel(ui_form_logging_widget);
  ui_downwards_lbl->setText(tr("Log goes downwards:"));
  ui_downwards_lbl->setToolTip(
      tr("If ticked, new messages will appear at "
         "the bottom (like the OOC chatlog). The traditional "
         "(AO1) behaviour is equivalent to this being unticked."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_downwards_lbl);

  ui_downwards_cb = new QCheckBox(ui_form_logging_widget);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_downwards_cb);

  row += 1;
  ui_length_lbl = new QLabel(ui_form_logging_widget);
  ui_length_lbl->setText(tr("Log length:"));
  ui_length_lbl->setToolTip(tr(
      "The amount of message lines the IC chatlog will keep before "
      "deleting older message lines. A value of 0 or below counts as 'infinite'."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_length_lbl);

  ui_length_spinbox = new QSpinBox(ui_form_logging_widget);
  ui_length_spinbox->setSuffix(" lines");
  ui_length_spinbox->setMaximum(10000);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_length_spinbox);

  row += 1;
  ui_log_newline_lbl = new QLabel(ui_form_logging_widget);
  ui_log_newline_lbl->setText(tr("Log newline:"));
  ui_log_newline_lbl->setToolTip(
      tr("If ticked, new messages will appear separated, "
         "with the message coming on the next line after the name. "
         "When unticked, it displays it as 'name: message'."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_newline_lbl);

  ui_log_newline_cb = new QCheckBox(ui_form_logging_widget);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_newline_cb);

  row += 1;
  ui_log_margin_lbl = new QLabel(ui_form_logging_widget);
  ui_log_margin_lbl->setText(tr("Log margin:"));
  ui_log_margin_lbl->setToolTip(tr(
      "The distance in pixels between each entry in the IC log. "
      "Default: 0."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_margin_lbl);

  ui_log_margin_spinbox = new QSpinBox(ui_form_logging_widget);
  ui_log_margin_spinbox->setSuffix(" px");
  ui_log_margin_spinbox->setMaximum(1000);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_margin_spinbox);

  row += 1;
  ui_log_timestamp_lbl = new QLabel(ui_form_logging_widget);
  ui_log_timestamp_lbl->setText(tr("Log timestamp:"));
  ui_log_timestamp_lbl->setToolTip(
      tr("If ticked, log will contain a timestamp in UTC before the name."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_timestamp_lbl);

  ui_log_timestamp_cb = new QCheckBox(ui_form_logging_widget);

  connect(ui_log_timestamp_cb, &QCheckBox::stateChanged, this, &AOOptionsDialog::timestamp_cb_changed);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_timestamp_cb);

  row += 1;
  ui_log_timestamp_format_lbl = new QLabel(ui_form_logging_widget);
  ui_log_timestamp_format_lbl->setText(tr("Log timestamp format:\n") + QDateTime::currentDateTime().toString(ao_app->get_log_timestamp_format()));
  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_timestamp_format_lbl);

  ui_log_timestamp_format_combobox = new QComboBox(ui_form_logging_widget);
  ui_log_timestamp_format_combobox->setEditable(true);

  QString l_current_format = ao_app->get_log_timestamp_format();

  ui_log_timestamp_format_combobox->setCurrentText(l_current_format);
  ui_log_timestamp_format_combobox->addItem("h:mm:ss AP"); // 2:13:09 PM
  ui_log_timestamp_format_combobox->addItem("hh:mm:ss"); // 14:13:09
  ui_log_timestamp_format_combobox->addItem("h:mm AP"); // 2:13 PM
  ui_log_timestamp_format_combobox->addItem("hh:mm"); // 14:13

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_timestamp_format_combobox);

  connect(ui_log_timestamp_format_combobox, &QComboBox::currentTextChanged, this, &AOOptionsDialog::on_timestamp_format_edited);

  if(!ao_app->get_log_timestamp()) {
    ui_log_timestamp_format_combobox->setDisabled(true);
  }
  row += 1;
  ui_log_ic_actions_lbl = new QLabel(ui_form_logging_widget);
  ui_log_ic_actions_lbl->setText(tr("Log IC actions:"));
  ui_log_ic_actions_lbl->setToolTip(
      tr("If ticked, log will show IC actions such as shouting and presenting evidence."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_ic_actions_lbl);

  ui_log_ic_actions_cb = new QCheckBox(ui_form_logging_widget);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_ic_actions_cb);

  row += 1;
  ui_desync_logs_lbl = new QLabel(ui_form_logging_widget);
  ui_desync_logs_lbl->setText(tr("Desynchronize IC Logs:"));
  ui_desync_logs_lbl->setToolTip(
      tr("If ticked, log will show messages as-received, while viewport will parse according to the queue (Text Stay Time)."));

  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_desync_logs_lbl);

  ui_desync_logs_cb = new QCheckBox(ui_form_logging_widget);

  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_desync_logs_cb);

  //Check whether mass logging is enabled
  row += 1;
  ui_log_text_lbl = new QLabel(ui_form_logging_widget);
  ui_log_text_lbl->setText(tr("Log to Text Files:"));
  ui_log_text_lbl->setToolTip(
      tr("Text logs of gameplay will be automatically written in the /logs folder."));
  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_text_lbl);

  ui_log_text_cb = new QCheckBox(ui_form_logging_widget);
  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_text_cb);

  row += 1;
  ui_log_demo_lbl = new QLabel(ui_form_logging_widget);
  ui_log_demo_lbl->setText(tr("Log to Demo Files:"));
  ui_log_demo_lbl->setToolTip(
      tr("Gameplay will be automatically recorded as demos in the /logs folder."));
  ui_logging_form->setWidget(row, QFormLayout::LabelRole, ui_log_demo_lbl);

  ui_log_demo_cb = new QCheckBox(ui_form_logging_widget);
  ui_logging_form->setWidget(row, QFormLayout::FieldRole, ui_log_demo_cb);

  // Finish logging tab
  QScrollArea *log_scroll = new QScrollArea(this);
  log_scroll->setWidget(ui_form_logging_widget);
  ui_logging_tab->setLayout(new QVBoxLayout);
  ui_logging_tab->layout()->addWidget(log_scroll);
  ui_logging_tab->show();

  // Privacy tab
  ui_privacy_tab = new QWidget(this);
  ui_settings_tabs->addTab(ui_privacy_tab, tr("Privacy"));

  ui_privacy_layout = new QVBoxLayout(ui_privacy_tab);

  ui_privacy_optout_cb = new QCheckBox(ui_privacy_tab);
  ui_privacy_optout_cb->setText(tr("Do not include me in public player counts"));
  ui_privacy_layout->addWidget(ui_privacy_optout_cb);

  ui_privacy_separator = new QFrame(ui_privacy_tab);
  ui_privacy_separator->setObjectName(QString::fromUtf8("line"));
  ui_privacy_separator->setFrameShape(QFrame::HLine);
  ui_privacy_separator->setFrameShadow(QFrame::Sunken);
  ui_privacy_layout->addWidget(ui_privacy_separator);

  ui_privacy_policy = new QTextBrowser(ui_privacy_tab);
  QSizePolicy privacySizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  ui_privacy_policy->setSizePolicy(privacySizePolicy);
  ui_privacy_policy->setPlainText(tr("Getting privacy policy..."));
  ui_privacy_layout->addWidget(ui_privacy_policy);

  update_values();

  // When we're done, we should continue the updates!
  setUpdatesEnabled(true);
}

void AOOptionsDialog::update_values() {
  for (int i = 0; i < ui_theme_combobox->count(); ++i) {
    if (ui_theme_combobox->itemText(i) == ao_app->read_theme())
    {
      ui_theme_combobox->setCurrentIndex(i);
      break;
    }
  }
  QString subtheme =
      ao_app->configini->value("subtheme").value<QString>();
  for (int i = 0; i < ui_subtheme_combobox->count(); ++i) {
    if (ui_subtheme_combobox->itemText(i) == subtheme)
    {
      ui_subtheme_combobox->setCurrentIndex(i);
      break;
    }
  }
  Qt::TransformationMode scaling = ao_app->get_scaling(ao_app->get_default_scaling());
  ui_scaling_combobox->setCurrentIndex(scaling);

  // Let's fill the callwords text edit with the already present callwords.
  ui_callwords_textbox->document()->clear();
  foreach (QString callword, ao_app->get_call_words()) {
    ui_callwords_textbox->appendPlainText(callword);
  }
  ui_animated_theme_cb->setChecked(ao_app->get_animated_theme());
  ui_ms_textbox->setText(ao_app->configini->value("master", "").value<QString>());
  ui_casing_cm_cases_textbox->setText(ao_app->get_casing_can_host_cases());
  ui_username_textbox->setText(ao_app->get_default_username());
  ui_downwards_cb->setChecked(ao_app->get_log_goes_downwards());
  ui_log_newline_cb->setChecked(ao_app->get_log_newline());
  ui_log_timestamp_cb->setChecked(ao_app->get_log_timestamp());
  ui_log_timestamp_format_combobox->setCurrentText(ao_app->get_log_timestamp_format());
  ui_log_ic_actions_cb->setChecked(ao_app->get_log_ic_actions());
  ui_desync_logs_cb->setChecked(ao_app->is_desyncrhonized_logs_enabled());
  ui_instant_objection_cb->setChecked(ao_app->is_instant_objection_enabled());
  ui_showname_cb->setChecked(ao_app->get_showname_enabled_by_default());
  ui_discord_cb->setChecked(ao_app->is_discord_enabled());
  ui_shake_cb->setChecked(ao_app->is_shake_enabled());
  ui_effects_cb->setChecked(ao_app->is_effects_enabled());
  ui_framenetwork_cb->setChecked(ao_app->is_frame_network_enabled());
  ui_colorlog_cb->setChecked(ao_app->is_colorlog_enabled());
  ui_stickysounds_cb->setChecked(ao_app->is_stickysounds_enabled());
  ui_stickyeffects_cb->setChecked(ao_app->is_stickyeffects_enabled());
  ui_stickypres_cb->setChecked(ao_app->is_stickypres_enabled());
  ui_customchat_cb->setChecked(ao_app->is_customchat_enabled());
  ui_sticker_cb->setChecked(ao_app->is_sticker_enabled());
  ui_continuous_cb->setChecked(ao_app->is_continuous_enabled());
  ui_category_stop_cb->setChecked(ao_app->is_category_stop_enabled());
  ui_blank_blips_cb->setChecked(ao_app->get_blank_blip());
  ui_loopsfx_cb->setChecked(ao_app->get_looping_sfx());
  ui_objectmusic_cb->setChecked(ao_app->objection_stop_music());
  ui_casing_enabled_cb->setChecked(ao_app->get_casing_enabled());
  ui_casing_def_cb->setChecked(ao_app->get_casing_defence_enabled());
  ui_casing_pro_cb->setChecked(ao_app->get_casing_prosecution_enabled());
  ui_casing_jud_cb->setChecked(ao_app->get_casing_judge_enabled());
  ui_casing_jur_cb->setChecked(ao_app->get_casing_juror_enabled());
  ui_casing_steno_cb->setChecked(ao_app->get_casing_steno_enabled());
  ui_casing_cm_cb->setChecked(ao_app->get_casing_cm_enabled());
  ui_log_text_cb->setChecked(ao_app->get_text_logging_enabled());
  ui_log_demo_cb->setChecked(ao_app->get_demo_logging_enabled());
  ui_length_spinbox->setValue(ao_app->get_max_log_size());
  ui_log_margin_spinbox->setValue(ao_app->get_log_margin());
  ui_stay_time_spinbox->setValue(ao_app->stay_time());
  ui_text_crawl_spinbox->setValue(ao_app->get_text_crawl());
  ui_chat_ratelimit_spinbox->setValue(ao_app->get_chat_ratelimit());
  ui_music_volume_spinbox->setValue(ao_app->get_default_music());
  ui_sfx_volume_spinbox->setValue(ao_app->get_default_sfx());
  ui_blips_volume_spinbox->setValue(ao_app->get_default_blip());
  ui_bliprate_spinbox->setValue(ao_app->read_blip_rate());
  ui_default_showname_textbox->setText(ao_app->get_default_showname());

  auto *defaultMount = new QListWidgetItem(tr("%1 (default)")
                                           .arg(ao_app->get_base_path()));
  defaultMount->setFlags(Qt::ItemFlag::NoItemFlags);

  //Clear the list to prevent duplication of default entries.
  ui_mount_list->clear();
  ui_mount_list->addItem(defaultMount);
  ui_mount_list->addItems(ao_app->get_mount_paths());

  ui_privacy_optout_cb->setChecked(ao_app->get_player_count_optout());

  ao_app->net_manager->request_document(MSDocumentType::PrivacyPolicy, [this](QString document) {
    if (document.isEmpty()) {
      document = tr("Couldn't get the privacy policy.");
    }
    ui_privacy_policy->setHtml(document);
  });
}

void AOOptionsDialog::save_pressed()
{
  // Save everything into the config.ini.
  QSettings *configini = ao_app->configini;

  const bool audioChanged = ui_audio_device_combobox->currentText() !=
      ao_app->get_audio_output_device();

  configini->setValue("theme", ui_theme_combobox->currentText());
  configini->setValue("subtheme", ui_subtheme_combobox->currentText());
  configini->setValue("animated_theme", ui_animated_theme_cb->isChecked());
  configini->setValue("log_goes_downwards", ui_downwards_cb->isChecked());
  configini->setValue("log_maximum", ui_length_spinbox->value());
  configini->setValue("log_newline", ui_log_newline_cb->isChecked());
  configini->setValue("log_margin", ui_log_margin_spinbox->value());
  configini->setValue("log_timestamp", ui_log_timestamp_cb->isChecked());
  configini->setValue("log_timestamp_format", ui_log_timestamp_format_combobox->currentText());
  configini->setValue("log_ic_actions", ui_log_ic_actions_cb->isChecked());
  configini->setValue("desync_logs", ui_desync_logs_cb->isChecked());
  configini->setValue("stay_time", ui_stay_time_spinbox->value());
  configini->setValue("instant_objection", ui_instant_objection_cb->isChecked());
  configini->setValue("text_crawl", ui_text_crawl_spinbox->value());
  configini->setValue("chat_ratelimit", ui_chat_ratelimit_spinbox->value());
  configini->setValue("default_username", ui_username_textbox->text());
  configini->setValue("show_custom_shownames", ui_showname_cb->isChecked());
  configini->setValue("default_showname", ui_default_showname_textbox->text());
  configini->setValue("master", ui_ms_textbox->text());
  configini->setValue("discord", ui_discord_cb->isChecked());
  configini->setValue("language", ui_language_combobox->currentText().left(2));
  configini->setValue("default_scaling", ui_scaling_combobox->currentData());
  configini->setValue("shake", ui_shake_cb->isChecked());
  configini->setValue("effects", ui_effects_cb->isChecked());
  configini->setValue("framenetwork", ui_framenetwork_cb->isChecked());
  configini->setValue("colorlog", ui_colorlog_cb->isChecked());
  configini->setValue("stickysounds", ui_stickysounds_cb->isChecked());
  configini->setValue("stickyeffects", ui_stickyeffects_cb->isChecked());
  configini->setValue("stickypres", ui_stickypres_cb->isChecked());
  configini->setValue("customchat", ui_customchat_cb->isChecked());
  configini->setValue("sticker", ui_sticker_cb->isChecked());
  configini->setValue("automatic_logging_enabled", ui_log_text_cb->isChecked());
  configini->setValue("demo_logging_enabled", ui_log_demo_cb->isChecked());
  configini->setValue("continuous_playback", ui_continuous_cb->isChecked());
  configini->setValue("category_stop", ui_category_stop_cb->isChecked());
  QFile *callwordsini = new QFile(ao_app->get_base_path() + "callwords.ini");

  if (callwordsini->open(QIODevice::WriteOnly | QIODevice::Truncate |
                         QIODevice::Text)) {
    QTextStream out(callwordsini);
    out.setCodec("UTF-8");
    out << ui_callwords_textbox->toPlainText();
    callwordsini->close();
  }

  configini->setValue("default_audio_device",
                      ui_audio_device_combobox->currentText());
  configini->setValue("default_music", ui_music_volume_spinbox->value());
  configini->setValue("default_sfx", ui_sfx_volume_spinbox->value());
  configini->setValue("default_blip", ui_blips_volume_spinbox->value());
  configini->setValue("blip_rate", ui_bliprate_spinbox->value());
  configini->setValue("blank_blip", ui_blank_blips_cb->isChecked());
  configini->setValue("looping_sfx", ui_loopsfx_cb->isChecked());
  configini->setValue("objection_stop_music", ui_objectmusic_cb->isChecked());

  configini->setValue("casing_enabled", ui_casing_enabled_cb->isChecked());
  configini->setValue("casing_defence_enabled", ui_casing_def_cb->isChecked());
  configini->setValue("casing_prosecution_enabled",
                      ui_casing_pro_cb->isChecked());
  configini->setValue("casing_judge_enabled", ui_casing_jud_cb->isChecked());
  configini->setValue("casing_juror_enabled", ui_casing_jur_cb->isChecked());
  configini->setValue("casing_steno_enabled", ui_casing_steno_cb->isChecked());
  configini->setValue("casing_cm_enabled", ui_casing_cm_cb->isChecked());
  configini->setValue("casing_can_host_cases",
                      ui_casing_cm_cases_textbox->text());
  configini->setValue("player_count_optout", ui_privacy_optout_cb->isChecked());

  QStringList mountPaths;
  for (int i = 1; i < ui_mount_list->count(); i++)
    mountPaths.append(ui_mount_list->item(i)->text());
  configini->setValue("mount_paths", mountPaths);

  if (audioChanged)
    ao_app->initBASS();

  if (asset_cache_dirty)
    ao_app->invalidate_lookup_cache();

  // We most probably pressed "Restore defaults" at some point. Since we're saving our settings, remove the temporary file.
  if (QFile::exists(ao_app->get_base_path() + "config.temp"))
      QFile::remove(ao_app->get_base_path() + "config.temp");
  done(0);
}

void AOOptionsDialog::discard_pressed() {
    // The .temp file exists, meaning we are trying to undo the user clicking on "Restore defaults" and bring back the old settings.
    if (QFile::exists(ao_app->get_base_path() + "config.temp")) {
        // Delete the QSettings object so it does not interfere with the file
        delete ao_app->configini;
        // Remove the current config.ini
        QFile::remove(ao_app->get_base_path() + "config.ini");
        // Rename .temp to .ini
        QFile::rename(ao_app->get_base_path() + "config.temp", ao_app->get_base_path() + "config.ini");
        // Recreate the QSettings object from the ini file, restoring the settings before the Options Dialog was opened..
        ao_app->configini =
            new QSettings(ao_app->get_base_path() + "config.ini", QSettings::IniFormat);
    }
    done(0);
}

void AOOptionsDialog::button_clicked(QAbstractButton *button) {
    if (ui_settings_buttons->buttonRole(button) == QDialogButtonBox::ResetRole) {
        // Store the current settings as a .temp file
        QFile::rename(ao_app->get_base_path() + "config.ini", ao_app->get_base_path() + "config.temp");
        // Load up the default settings
        ao_app->configini->clear();
        // Update the values on the settings ui
        update_values();
    }
}

void AOOptionsDialog::on_reload_theme_clicked() {
    ao_app->configini->setValue("theme", ui_theme_combobox->currentText());
    ao_app->configini->setValue("subtheme", ui_subtheme_combobox->currentText());
    ao_app->configini->setValue("animated_theme", ui_animated_theme_cb->isChecked());
    if (ao_app->courtroom_constructed)
        ao_app->w_courtroom->on_reload_theme_clicked();
    if (ao_app->lobby_constructed)
        ao_app->w_lobby->set_widgets();
}

void AOOptionsDialog::theme_changed(int i) {
  ui_subtheme_combobox->clear();
  // Fill the combobox with the names of the themes.
  ui_subtheme_combobox->addItem("server");
  ui_subtheme_combobox->addItem("default");
  QDirIterator it(ao_app->get_real_path(ao_app->get_theme_path("", ui_theme_combobox->itemText(i))), QDir::Dirs,
                  QDirIterator::NoIteratorFlags);
  while (it.hasNext()) {
    QString actualname = QDir(it.next()).dirName();
    if (actualname != "." && actualname != ".." && actualname.toLower() != "server" && actualname.toLower() != "default" && actualname.toLower() != "effects" && actualname.toLower() != "misc")
      ui_subtheme_combobox->addItem(actualname);
  }

}

void AOOptionsDialog::about_button_pressed()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");

  QString msg =
      tr("<h2>kenji %1</h2>"
         "When you've run into a wall with no place to go, return to the basics."
         "<p><b>Source code:</b> "
         "<a href='https://github.com/in1tiate/kenji'>"
         "https://github.com/in1tiate/kenji</a>"
         "<h2>Attorney Online 2:</h2>"
         "The courtroom drama simulator."
         "<p><b>AO2 Source code:</b> "
         "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
         "https://github.com/AttorneyOnline/AO2-Client</a>"
         "<p><b>Major development:</b><br>"
         "OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, "
         "Crystalwarrior, Iamgoofball, in1tiate"
         "<p><b>Client development:</b><br>"
         "Cents02, windrammer, skyedeving"
         "<p><b>QA testing:</b><br>"
         "CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, "
         "Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, "
         "Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, "
         "Veritas, Wiso"
         "<p><b>Translations:</b><br>"
         "k-emiko (Русский), Pyraq (Polski), scatterflower (日本語), vintprox (Русский), "
         "windrammer (Español, Português)"
         "<p><b>Special thanks:</b><br>"
         "CrazyJC (2.8 release director) and MaximumVolty (2.8 release promotion); "
         "Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); "
         "Rue (website); Draxirch (UI design); "
         "Lewdton and Argoneus (tsuserver); "
         "Fiercy, Noevain, Cronnicossy, and FanatSors (AO1); "
         "server hosts, game masters, case makers, content creators, "
         "and the whole AO2 community!"
         "<p>The Attorney Online networked visual novel project "
         "is copyright (c) 2016-2021 Attorney Online developers. Open-source "
         "licenses apply. All other assets are the property of their "
         "respective owners."
         "<p>Running on Qt version %2 with the BASS audio engine.<br>"
         "APNG plugin loaded: %3"
         "<p>Built on %4")
      .arg(ao_app->get_version_string())
      .arg(QLatin1String(QT_VERSION_STR))
      .arg(hasApng ? tr("Yes") : tr("No"))
      .arg(QLatin1String(__DATE__));
  QMessageBox::about(this, tr("About"), msg);
}

void AOOptionsDialog::on_timestamp_format_edited() { ui_log_timestamp_format_lbl->setText(tr("Log timestamp format:\n") + QDateTime::currentDateTime().toString(ui_log_timestamp_format_combobox->currentText())); }

void AOOptionsDialog::timestamp_cb_changed(int state) { ui_log_timestamp_format_combobox->setDisabled(state == 0); }

#if (defined(_WIN32) || defined(_WIN64))
bool AOOptionsDialog::needs_default_audiodev() { return true; }
#elif (defined(LINUX) || defined(__linux__))
bool AOOptionsDialog::needs_default_audiodev() { return false; }
#elif defined __APPLE__
bool AOOptionsDialog::needs_default_audiodev() { return true; }
#else
#error This operating system is not supported.
#endif
