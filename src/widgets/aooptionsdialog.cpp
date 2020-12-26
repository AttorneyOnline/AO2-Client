#include "widgets/aooptionsdialog.h"
#include "aoapplication.h"
#include "contrib/bass.h"

AOOptionsDialog::AOOptionsDialog(QWidget *parent, AOApplication *p_ao_app) : QDialog(parent)
{
    ao_app = p_ao_app;

    // Setting up the basics.
    // setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Settings"));
    resize(398, 320);

    ui_settings_buttons = new QDialogButtonBox(this);

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(ui_settings_buttons->sizePolicy().hasHeightForWidth());
    ui_settings_buttons->setSizePolicy(sizePolicy1);
    ui_settings_buttons->setOrientation(Qt::Horizontal);
    ui_settings_buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Save);

    QObject::connect(ui_settings_buttons, SIGNAL(accepted()), this, SLOT(save_pressed()));
    QObject::connect(ui_settings_buttons, SIGNAL(rejected()), this, SLOT(discard_pressed()));

    // We'll stop updates so that the window won't flicker while it's being made.
    setUpdatesEnabled(false);

    // First of all, we want a tabbed dialog, so let's add some layout.
    ui_vertical_layout = new QVBoxLayout(this);
    ui_settings_tabs = new QTabWidget(this);

    ui_vertical_layout->addWidget(ui_settings_tabs);
    ui_vertical_layout->addWidget(ui_settings_buttons);

    // Let's add the tabs one by one.
    // First, we'll start with 'Gameplay'.
    ui_gameplay_tab = new QWidget(ui_settings_tabs);
    ui_settings_tabs->addTab(ui_gameplay_tab, tr("Gameplay"));

    ui_form_layout_widget = new QWidget(ui_gameplay_tab);
    ui_form_layout_widget->setGeometry(QRect(10, 10, 361, 211));

    ui_gameplay_form = new QFormLayout(ui_form_layout_widget);
    ui_gameplay_form->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    ui_gameplay_form->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    ui_gameplay_form->setContentsMargins(0, 0, 0, 0);

    ui_theme_label = new QLabel(ui_form_layout_widget);
    ui_theme_label->setText(tr("Theme:"));
    ui_theme_label->setToolTip(tr("Sets the theme used in-game. If the new theme changes "
                                  "the lobby's look as well, you'll need to reload the "
                                  "lobby for the changes to take effect, such as by joining "
                                  "a server and leaving it."));
    ui_gameplay_form->setWidget(0, QFormLayout::LabelRole, ui_theme_label);

    ui_theme_combobox = new QComboBox(ui_form_layout_widget);

    // Fill the combobox with the names of the themes.
    QDirIterator it(p_ao_app->get_base_path() + "themes", QDir::Dirs, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        QString actualname = QDir(it.next()).dirName();
        if (actualname != "." && actualname != "..")
            ui_theme_combobox->addItem(actualname);
        if (actualname == options.theme())
            ui_theme_combobox->setCurrentIndex(ui_theme_combobox->count()-1);
    }

    ui_gameplay_form->setWidget(0, QFormLayout::FieldRole, ui_theme_combobox);

    ui_theme_log_divider = new QFrame(ui_form_layout_widget);
    ui_theme_log_divider->setMidLineWidth(0);
    ui_theme_log_divider->setFrameShape(QFrame::HLine);
    ui_theme_log_divider->setFrameShadow(QFrame::Sunken);

    ui_gameplay_form->setWidget(1, QFormLayout::FieldRole, ui_theme_log_divider);

    ui_downwards_lbl = new QLabel(ui_form_layout_widget);
    ui_downwards_lbl->setText(tr("Legacy scroll:"));
    ui_downwards_lbl->setToolTip(tr("If ticked, new messages will appear at the top."));

    ui_gameplay_form->setWidget(2, QFormLayout::LabelRole, ui_downwards_lbl);

    ui_downwards_cb = new QCheckBox(ui_form_layout_widget);
    ui_downwards_cb->setChecked(options.legacyScrollEnabled());

    ui_gameplay_form->setWidget(2, QFormLayout::FieldRole, ui_downwards_cb);

    ui_length_lbl = new QLabel(ui_form_layout_widget);
    ui_length_lbl->setText(tr("Log length:"));
    ui_length_lbl->setToolTip(tr("The amount of messages the IC chatlog will keep before "
                                 "deleting older messages. A value of 0 or below counts as 'infinite'."));

    ui_gameplay_form->setWidget(3, QFormLayout::LabelRole, ui_length_lbl);

    ui_length_spinbox = new QSpinBox(ui_form_layout_widget);
    ui_length_spinbox->setMaximum(10000);
    ui_length_spinbox->setValue(options.maxLogLines());

    ui_gameplay_form->setWidget(3, QFormLayout::FieldRole, ui_length_spinbox);

    ui_log_names_divider = new QFrame(ui_form_layout_widget);
    ui_log_names_divider->setFrameShape(QFrame::HLine);
    ui_log_names_divider->setFrameShadow(QFrame::Sunken);

    ui_gameplay_form->setWidget(4, QFormLayout::FieldRole, ui_log_names_divider);

    ui_username_lbl = new QLabel(ui_form_layout_widget);
    ui_username_lbl->setText(tr("Default username:"));
    ui_username_lbl->setToolTip(tr("Your OOC name will be automatically set to this value "
                                   "when you join a server."));

    ui_gameplay_form->setWidget(5, QFormLayout::LabelRole, ui_username_lbl);

    ui_username_textbox = new QLineEdit(ui_form_layout_widget);
    ui_username_textbox->setMaxLength(30);
    ui_username_textbox->setText(options.oocName());

    ui_gameplay_form->setWidget(5, QFormLayout::FieldRole, ui_username_textbox);

    ui_showname_lbl = new QLabel(ui_form_layout_widget);
    ui_showname_lbl->setText(tr("Custom shownames:"));
    ui_showname_lbl->setToolTip(tr("Gives the default value for the in-game 'Custom shownames' "
                                   "tickbox, which in turn determines whether the client should "
                                   "display custom in-character names."));

    ui_gameplay_form->setWidget(6, QFormLayout::LabelRole, ui_showname_lbl);

    ui_showname_cb = new QCheckBox(ui_form_layout_widget);
    ui_showname_cb->setChecked(options.shownamesEnabled());

    ui_gameplay_form->setWidget(6, QFormLayout::FieldRole, ui_showname_cb);

    ui_net_divider = new QFrame(ui_form_layout_widget);
    ui_net_divider->setFrameShape(QFrame::HLine);
    ui_net_divider->setFrameShadow(QFrame::Sunken);

    ui_gameplay_form->setWidget(7, QFormLayout::FieldRole, ui_net_divider);

    ui_ms_lbl = new QLabel(ui_form_layout_widget);
    ui_ms_lbl->setText(tr("Backup MS:"));
    ui_ms_lbl->setToolTip(tr("If the built-in server lookups fail, the game will try the "
                             "address given here and use it as a backup master server address."));

    ui_gameplay_form->setWidget(8, QFormLayout::LabelRole, ui_ms_lbl);

    ui_ms_textbox = new QLineEdit(ui_form_layout_widget);
    ui_ms_textbox->setText(options.msAddress());

    ui_gameplay_form->setWidget(8, QFormLayout::FieldRole, ui_ms_textbox);

    ui_discord_lbl = new QLabel(ui_form_layout_widget);
    ui_discord_lbl->setText(tr("Discord:"));
    ui_discord_lbl->setToolTip(tr("Allows others on Discord to see what server you are in, "
                                  "what character are you playing, and how long you have "
                                  "been playing for."));

    ui_gameplay_form->setWidget(9, QFormLayout::LabelRole, ui_discord_lbl);

    ui_discord_cb = new QCheckBox(ui_form_layout_widget);
    ui_discord_cb->setChecked(options.discordEnabled());

    ui_gameplay_form->setWidget(9, QFormLayout::FieldRole, ui_discord_cb);

    // Here we start the callwords tab.
    ui_callwords_tab = new QWidget(ui_settings_tabs);
    ui_settings_tabs->addTab(ui_callwords_tab, tr("Callwords"));

    ui_callwords_widget = new QWidget(ui_callwords_tab);
    ui_callwords_widget->setGeometry(QRect(10, 10, 361, 211));

    ui_callwords_layout = new QVBoxLayout(ui_callwords_widget);
    ui_callwords_layout->setContentsMargins(0,0,0,0);

    ui_callwords_textbox = new QPlainTextEdit(ui_callwords_widget);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ui_callwords_textbox->sizePolicy().hasHeightForWidth());
    ui_callwords_textbox->setSizePolicy(sizePolicy);

    // Let's fill the callwords text edit with the already present callwords.
    ui_callwords_textbox->document()->clear();
    for (const QString &callword : options.callWords()) {
        ui_callwords_textbox->appendPlainText(callword);
    }

    ui_callwords_layout->addWidget(ui_callwords_textbox);

    ui_callwords_explain_lbl = new QLabel(ui_callwords_widget);
    ui_callwords_explain_lbl->setWordWrap(true);
    ui_callwords_explain_lbl->setText(tr("<html><head/><body>Enter as many callwords as you would like. These are case insensitive. Make sure to leave every callword in its own line!<br>Do not leave a line with a space at the end -- you will be alerted everytime someone uses a space in their messages.</body></html>"));

    ui_callwords_layout->addWidget(ui_callwords_explain_lbl);

    // The audio tab.
    ui_audio_tab = new QWidget(ui_settings_tabs);
    ui_settings_tabs->addTab(ui_audio_tab, tr("Audio"));

    ui_audio_widget = new QWidget(ui_audio_tab);
    ui_audio_widget->setGeometry(QRect(10, 10, 361, 211));

    ui_audio_layout = new QFormLayout(ui_audio_widget);
    ui_audio_layout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    ui_audio_layout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    ui_audio_layout->setContentsMargins(0, 0, 0, 0);

    ui_audio_device_lbl = new QLabel(ui_audio_widget);
    ui_audio_device_lbl->setText(tr("Audio device:"));
    ui_audio_device_lbl->setToolTip(tr("Sets the audio device for all sounds."));

    ui_audio_layout->setWidget(0, QFormLayout::LabelRole, ui_audio_device_lbl);

    ui_audio_device_combobox = new QComboBox(ui_audio_widget);

    // Let's fill out the combobox with the available audio devices.
    int a = 0;
    BASS_DEVICEINFO info;

    if (needs_default_audiodev())
    {
        ui_audio_device_combobox->addItem("default");
    }

    for (a = 0; BASS_GetDeviceInfo(a, &info); a++)
    {
        ui_audio_device_combobox->addItem(info.name);
        if (options.audioDevice() == info.name)
            ui_audio_device_combobox->setCurrentIndex(ui_audio_device_combobox->count()-1);
    }

    ui_audio_layout->setWidget(0, QFormLayout::FieldRole, ui_audio_device_combobox);

    ui_audio_volume_divider = new QFrame(ui_audio_widget);
    ui_audio_volume_divider->setFrameShape(QFrame::HLine);
    ui_audio_volume_divider->setFrameShadow(QFrame::Sunken);

    ui_audio_layout->setWidget(1, QFormLayout::FieldRole, ui_audio_volume_divider);

    ui_music_volume_lbl = new QLabel(ui_audio_widget);
    ui_music_volume_lbl->setText(tr("Music:"));
    ui_music_volume_lbl->setToolTip(tr("Sets the music's default volume."));

    ui_audio_layout->setWidget(2, QFormLayout::LabelRole, ui_music_volume_lbl);

    ui_music_volume_spinbox = new QSpinBox(ui_audio_widget);
    ui_music_volume_spinbox->setValue(options.defaultMusicVolume());
    ui_music_volume_spinbox->setMaximum(100);
    ui_music_volume_spinbox->setSuffix("%");

    ui_audio_layout->setWidget(2, QFormLayout::FieldRole, ui_music_volume_spinbox);

    ui_sfx_volume_lbl = new QLabel(ui_audio_widget);
    ui_sfx_volume_lbl->setText(tr("SFX:"));
    ui_sfx_volume_lbl->setToolTip(tr("Sets the SFX's default volume. "
                                     "Interjections and actual sound effects count as 'SFX'."));

    ui_audio_layout->setWidget(3, QFormLayout::LabelRole, ui_sfx_volume_lbl);

    ui_sfx_volume_spinbox = new QSpinBox(ui_audio_widget);
    ui_sfx_volume_spinbox->setValue(options.defaultSfxVolume());
    ui_sfx_volume_spinbox->setMaximum(100);
    ui_sfx_volume_spinbox->setSuffix("%");

    ui_audio_layout->setWidget(3, QFormLayout::FieldRole, ui_sfx_volume_spinbox);

    ui_blips_volume_lbl = new QLabel(ui_audio_widget);
    ui_blips_volume_lbl->setText(tr("Blips:"));
    ui_blips_volume_lbl->setToolTip(tr("Sets the volume of the blips, the talking sound effects."));

    ui_audio_layout->setWidget(4, QFormLayout::LabelRole, ui_blips_volume_lbl);

    ui_blips_volume_spinbox = new QSpinBox(ui_audio_widget);
    ui_blips_volume_spinbox->setValue(options.defaultBlipVolume());
    ui_blips_volume_spinbox->setMaximum(100);
    ui_blips_volume_spinbox->setSuffix("%");

    ui_audio_layout->setWidget(4, QFormLayout::FieldRole, ui_blips_volume_spinbox);

    ui_volume_blip_divider = new QFrame(ui_audio_widget);
    ui_volume_blip_divider->setFrameShape(QFrame::HLine);
    ui_volume_blip_divider->setFrameShadow(QFrame::Sunken);

    ui_audio_layout->setWidget(5, QFormLayout::FieldRole, ui_volume_blip_divider);

    ui_bliprate_lbl = new QLabel(ui_audio_widget);
    ui_bliprate_lbl->setText(tr("Blip rate:"));
    ui_bliprate_lbl->setToolTip(tr("Sets the delay between playing the blip sounds."));

    ui_audio_layout->setWidget(6, QFormLayout::LabelRole, ui_bliprate_lbl);

    ui_bliprate_spinbox = new QSpinBox(ui_audio_widget);
    ui_bliprate_spinbox->setValue(options.blipRate());
    ui_bliprate_spinbox->setMinimum(1);

    ui_audio_layout->setWidget(6, QFormLayout::FieldRole, ui_bliprate_spinbox);

    ui_blank_blips_lbl = new QLabel(ui_audio_widget);
    ui_blank_blips_lbl->setText(tr("Blank blips:"));
    ui_blank_blips_lbl->setToolTip(tr("If true, the game will play a blip sound even "
                                      "when a space is 'being said'."));

    ui_audio_layout->setWidget(7, QFormLayout::LabelRole, ui_blank_blips_lbl);

    ui_blank_blips_cb = new QCheckBox(ui_audio_widget);
    ui_blank_blips_cb->setChecked(options.blankBlipsEnabled());

    ui_audio_layout->setWidget(7, QFormLayout::FieldRole, ui_blank_blips_cb);

    // When we're done, we should continue the updates!
    setUpdatesEnabled(true);
}

void AOOptionsDialog::save_pressed()
{
    // Save everything into the config.ini.
    QSettings* configini = ao_app->configini;

    configini->setValue("theme", ui_theme_combobox->currentText());
    configini->setValue("log_goes_downwards", ui_downwards_cb->isChecked());
    configini->setValue("log_maximum", ui_length_spinbox->value());
    configini->setValue("default_username", ui_username_textbox->text());
    configini->setValue("show_custom_shownames", ui_showname_cb->isChecked());
    configini->setValue("master", ui_ms_textbox->text());
    configini->setValue("discord", ui_discord_cb->isChecked());

    QFile callwordsini(ao_app->get_base_path() + "callwords.ini");

    if (!callwordsini.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        // Nevermind!
    }
    else
    {
        QTextStream out(&callwordsini);
        out << ui_callwords_textbox->toPlainText();
        callwordsini.close();
    }

    configini->setValue("default_audio_device", ui_audio_device_combobox->currentText());
    configini->setValue("default_music", ui_music_volume_spinbox->value());
    configini->setValue("default_sfx", ui_sfx_volume_spinbox->value());
    configini->setValue("default_blip", ui_blips_volume_spinbox->value());
    configini->setValue("blip_rate", ui_bliprate_spinbox->value());
    configini->setValue("blank_blip", ui_blank_blips_cb->isChecked());

    done(0);
}

void AOOptionsDialog::discard_pressed()
{
    done(0);
}

#if (defined (_WIN32) || defined (_WIN64))
bool AOOptionsDialog::needs_default_audiodev()
{
    return true;
}
#elif (defined (LINUX) || defined (__linux__))
bool AOOptionsDialog::needs_default_audiodev()
{
    return false;
}
#else
#error This operating system is not supported.
#endif
