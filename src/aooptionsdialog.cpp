#include "aooptionsdialog.h"
#include "aoapplication.h"
#include "courtroom.h"
#include "lobby.h"
#include "bass.h"
#include "networkmanager.h"
#include "options.h"
#include "aoapplication.h"

#include <QUiLoader>

#include <QFileDialog>
#include <QString>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>

AOOptionsDialog::AOOptionsDialog(QWidget *parent, AOApplication *p_ao_app)
    : QWidget(parent)
{
  ao_app = p_ao_app;

  QUiLoader l_loader(this);
  QFile l_uiFile(":/resource/ui/optionsdialogue.ui");
  if (!l_uiFile.open(QFile::ReadOnly)) {
    qWarning() << "Unable to open file " << l_uiFile.fileName();
    return;
  }

  ui_settings_widget = l_loader.load(&l_uiFile, this);

  auto l_layout = new QVBoxLayout(this);
  l_layout->addWidget(ui_settings_widget);

  FROM_UI(QDialogButtonBox, settings_buttons);

  connect(ui_settings_buttons, &QDialogButtonBox::accepted, this,
                   &AOOptionsDialog::save_pressed);
  connect(ui_settings_buttons, &QDialogButtonBox::rejected, this,
                   &AOOptionsDialog::discard_pressed);
  connect(ui_settings_buttons, &QDialogButtonBox::clicked, this,
                   &AOOptionsDialog::button_clicked);

  FROM_UI(QComboBox, theme_combobox)
  registerOption<QComboBox, QString>("theme_combobox", &Options::theme, &Options::setTheme);

  QSet<QString> themes;
  QStringList bases = Options::options->mountpaths();
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

  FROM_UI(QComboBox, subtheme_combobox)
  connect(ui_theme_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                   &AOOptionsDialog::theme_changed);
  registerOption<QComboBox, QString>("subtheme_combobox", &Options::subTheme, &Options::setSubTheme);

  QDirIterator it2(ao_app->get_real_path(ao_app->get_theme_path("")), QDir::Dirs,
                  QDirIterator::NoIteratorFlags);
  while (it2.hasNext()) {
    QString actualname = QDir(it2.next()).dirName();
    if (actualname != "." && actualname != ".." && actualname.toLower() != "server" && actualname.toLower() != "default" && actualname.toLower() != "effects" && actualname.toLower() != "misc") {
      ui_subtheme_combobox->addItem(actualname);
    }
  }

  FROM_UI(QPushButton, theme_reload_button)
  connect(ui_theme_reload_button, &QPushButton::clicked, this,
          &AOOptionsDialog::on_reload_theme_clicked);

  FROM_UI(QPushButton, theme_folder_button)
  connect(ui_theme_folder_button, &QPushButton::clicked, this,
    [=] {
      QString p_path = ao_app->get_real_path(ao_app->get_theme_path("", ui_theme_combobox->itemText(ui_theme_combobox->currentIndex())));
      if (!dir_exists(p_path)) {
        return;
      }
      QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
    }
  );

  FROM_UI(QCheckBox, animated_theme_cb)
  registerOption<QCheckBox, bool>("animated_theme_cb", &Options::animatedThemeEnabled, &Options::setAnimatedThemeEnabled);

  FROM_UI(QSpinBox, stay_time_spinbox)
  registerOption<QSpinBox, int>("stay_time_spinbox", &Options::textStayTime, &Options::setTextStayTime);

  FROM_UI(QCheckBox, instant_objection_cb)
  registerOption<QCheckBox, bool>("instant_objection_cb", &Options::objectionSkipQueueEnabled, &Options::setObjectionSkipQueueEnabled);

  FROM_UI(QSpinBox, text_crawl_spinbox)
  registerOption<QSpinBox, int>("text_crawl_spinbox", &Options::textCrawlSpeed, &Options::setTextCrawlSpeed);

  FROM_UI(QSpinBox, chat_ratelimit_spinbox)
  registerOption<QSpinBox, int>("chat_ratelimit_spinbox", &Options::chatRateLimit, &Options::setChatRateLimit);

  FROM_UI(QLineEdit, username_textbox)
  registerOption<QLineEdit, QString>("username_textbox", &Options::username, &Options::setUsername);

  FROM_UI(QCheckBox, showname_cb)
  registerOption<QCheckBox, bool>("showname_cb", &Options::customShownameEnabled, &Options::setCustomShownameEnabled);

  FROM_UI(QLineEdit, default_showname_textbox);
  registerOption<QLineEdit, QString>("default_showname_textbox", &Options::shownameOnJoin, &Options::setShownameOnJoin);

  FROM_UI(QLineEdit, ms_textbox)
  registerOption<QLineEdit, QString>("ms_textbox", &Options::alternativeMasterserver, &Options::setAlternativeMasterserver);

  FROM_UI(QCheckBox, discord_cb)
  registerOption<QCheckBox, bool>("discord_cb", &Options::discordEnabled, &Options::setDiscordEnabled);

  FROM_UI(QComboBox, language_combobox)
  registerOption<QComboBox, QString>("language_combobox", &Options::language, &Options::setLanguage);

  FROM_UI(QComboBox, scaling_combobox)
  registerOption<QComboBox, QString>("scaling_combobox", &Options::defaultScalingMode, &Options::setDefaultScalingMode);
  ui_scaling_combobox->addItem(tr("Pixel"), "fast");
  ui_scaling_combobox->addItem(tr("Smooth"), "smooth");

  FROM_UI(QCheckBox, shake_cb)
  registerOption<QCheckBox, bool>("shake_cb",  &Options::shakeEnabled, &Options::setShakeEnabled);

  FROM_UI(QCheckBox, effects_cb)
  registerOption<QCheckBox, bool>("effects_cb", &Options::effectsEnabled, &Options::setEffectsEnabled);

  FROM_UI(QCheckBox, framenetwork_cb)
  registerOption<QCheckBox, bool>("framenetwork_cb", &Options::networkedFrameSfxEnabled, &Options::setNetworkedFrameSfxEnabled);

  FROM_UI(QCheckBox, colorlog_cb)
  registerOption<QCheckBox, bool>("colorlog_cb", &Options::colorLogEnabled, &Options::setColorLogEnabled);

  FROM_UI(QCheckBox, stickysounds_cb)
  registerOption<QCheckBox, bool>("stickysounds_cb", &Options::clearSoundsDropdownOnPlayEnabled, &Options::setClearSoundsDropdownOnPlayEnabled);

  FROM_UI(QCheckBox, stickyeffects_cb)
  registerOption<QCheckBox, bool>("stickyeffects_cb", &Options::clearEffectsDropdownOnPlayEnabled, &Options::setClearEffectsDropdownOnPlayEnabled);

  FROM_UI(QCheckBox, stickypres_cb)
  registerOption<QCheckBox, bool>("stickypres_cb", &Options::clearPreOnPlayEnabled, &Options::setClearPreOnPlayEnabled);

  FROM_UI(QCheckBox, customchat_cb)
  registerOption<QCheckBox, bool>("customchat_cb", &Options::customChatboxEnabled, &Options::setCustomChatboxEnabled);

  FROM_UI(QCheckBox, sticker_cb)
  registerOption<QCheckBox, bool>("sticker_cb", &Options::characterStickerEnabled, &Options::setCharacterStickerEnabled);

  FROM_UI(QCheckBox, continuous_cb)
  registerOption<QCheckBox, bool>("continuous_cb", &Options::continuousPlaybackEnabled, &Options::setContinuousPlaybackEnabled);

  FROM_UI(QCheckBox, category_stop_cb)
  registerOption<QCheckBox, bool>("category_stop_cb", &Options::stopMusicOnCategoryEnabled, &Options::setStopMusicOnCategoryEnabled);

  FROM_UI(QCheckBox, sfx_on_idle_cb)
  registerOption<QCheckBox, bool>("sfx_on_idle_cb", &Options::SfxonIdle, &Options::setSfxOnIdle);

  FROM_UI(QCheckBox, evidence_double_click_cb)
  registerOption<QCheckBox, bool>("evidence_double_click_cb", &Options::evidenceDoubleClickEdit, &Options::setEvidenceDoubleClickEdit);

  FROM_UI(QPlainTextEdit, callwords_textbox)

  const QStringList temp_callword_list = options.callwords();
  for (const QString &callword : temp_callword_list) {
    ui_callwords_textbox->appendPlainText(callword);
  }

  FROM_UI(QComboBox, audio_device_combobox)
  populateAudioDevices();

  registerOption<QComboBox, QString>("audio_device_combobox", &Options::audioOutputDevice, &Options::setAudioOutputDevice);

  FROM_UI(QSpinBox, music_volume_spinbox)
  registerOption<QSpinBox, int>("music_volume_spinbox", &Options::musicVolume, &Options::setMusicVolume);

  FROM_UI(QSpinBox, sfx_volume_spinbox)
  registerOption<QSpinBox, int>("sfx_volume_spinbox", &Options::sfxVolume, &Options::setSfxVolume);

  FROM_UI(QSpinBox, blips_volume_spinbox)
  registerOption<QSpinBox, int>("blips_volume_spinbox", &::Options::blipVolume, &Options::setBlipVolume);

  FROM_UI(QSpinBox, suppress_audio_spinbox)
  registerOption<QSpinBox, int>("suppress_audio_spinbox", &::Options::defaultSuppressAudio, &Options::setDefaultSupressedAudio);

  FROM_UI(QSpinBox, bliprate_spinbox)
  registerOption<QSpinBox, int>("bliprate_spinbox", &::Options::blipRate, &Options::setBlipRate);

  FROM_UI(QCheckBox, blank_blips_cb)
  registerOption<QCheckBox, bool>("blank_blips_cb", &Options::blankBlip, &Options::setBlankBlip);

  FROM_UI(QCheckBox, loopsfx_cb)
  registerOption<QCheckBox, bool>("loopsfx_cb", &Options::loopingSfx, &Options::setLoopingSfx);

  FROM_UI(QCheckBox, objectmusic_cb)
  registerOption<QCheckBox, bool>("objectmusic_cb", &Options::objectionStopMusic, &Options::setObjectionStopMusic);

  FROM_UI(QCheckBox, disablestreams_cb)
  registerOption<QCheckBox, bool>("disablestreams_cb", &Options::streamingEnabled, &Options::setStreamingEnabled);

  FROM_UI(QGroupBox, casing_enabled_box)
  registerOption<QGroupBox, bool>("casing_enabled_box", &Options::casingAlertEnabled, &Options::setCasingAlertEnabled);

  FROM_UI(QCheckBox, casing_def_cb)
  registerOption<QCheckBox, bool>("casing_def_cb", &Options::casingDefenceEnabled, &Options::setcasingDefenceEnabled);

  FROM_UI(QCheckBox, casing_pro_cb)
  registerOption<QCheckBox, bool>("casing_pro_cb", &Options::casingProsecutionEnabled, &::Options::setCasingProseuctionEnabled);

  FROM_UI(QCheckBox, casing_jud_cb)
  registerOption<QCheckBox, bool>("casing_jud_cb", &Options::casingJudgeEnabled, &::Options::setCasingJudgeEnabled);

  FROM_UI(QCheckBox, casing_jur_cb)
  registerOption<QCheckBox, bool>("casing_jur_cb", &Options::casingJurorEnabled, &Options::setCasingJurorEnabled);

  FROM_UI(QCheckBox, casing_steno_cb)
  registerOption<QCheckBox, bool>("casing_steno_cb", &Options::casingStenoEnabled, &Options::setCasingStenoEnabled);

  FROM_UI(QCheckBox, casing_cm_cb)
  registerOption<QCheckBox, bool>("casing_cm_cb", &Options::casingCmEnabled, &Options::setCasingCmEnabled);

  FROM_UI(QLineEdit, casing_cm_cases_textbox)
  registerOption<QLineEdit, QString>("casing_cm_cases_textbox", &Options::casingCanHostCases, &Options::setCasingCanHostCases);

  FROM_UI(QListWidget, mount_list)
  auto *defaultMount = new QListWidgetItem(tr("%1 (default)")
                                           .arg(ao_app->get_base_path()));
  defaultMount->setFlags(Qt::ItemFlag::NoItemFlags);
  ui_mount_list->addItem(defaultMount);
  registerOption<QListWidget, QStringList>("mount_list", &Options::mountpaths, &Options::setMountpaths);

  FROM_UI(QPushButton, mount_add)
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

  FROM_UI(QPushButton, mount_remove)
  connect(ui_mount_remove, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
      return;
    delete selected[0];
    emit ui_mount_list->itemSelectionChanged();
    asset_cache_dirty = true;
  });

  FROM_UI(QPushButton, mount_up)
  connect(ui_mount_up, &QPushButton::clicked, this, [this] {
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

  FROM_UI(QPushButton, mount_down)
  connect(ui_mount_down, &QPushButton::clicked, this, [this] {
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

  FROM_UI(QPushButton, mount_clear_cache)
  connect(ui_mount_clear_cache, &QPushButton::clicked, this, [this] {
    asset_cache_dirty = true;
    ui_mount_clear_cache->setEnabled(false);
  });

  connect(ui_mount_list, &QListWidget::itemSelectionChanged, this, [this] {
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

  FROM_UI(QCheckBox, downwards_cb)
  registerOption<QCheckBox, bool>("downwards_cb", &Options::logDirectionDownwards, &Options::setLogDirectionDownwards);

  FROM_UI(QSpinBox, length_spinbox)
  registerOption<QSpinBox, int>("length_spinbox", &Options::maxLogSize, &Options::setMaxLogSize);

  FROM_UI(QCheckBox, log_newline_cb)
  registerOption<QCheckBox, bool>("log_newline_cb", &Options::logNewline, &Options::setLogNewline);

  FROM_UI(QSpinBox, log_margin_spinbox)
  registerOption<QSpinBox, int>("log_margin_spinbox", &Options::logMargin, &Options::setLogMargin);

  FROM_UI(QCheckBox, log_timestamp_cb)
  registerOption<QCheckBox, bool>("log_timestamp_cb", &Options::logTimestampEnabled, &Options::setLogTimestampEnabled);
  connect(ui_log_timestamp_cb, &QCheckBox::stateChanged, this, &AOOptionsDialog::timestamp_cb_changed);

  FROM_UI(QLabel, log_timestamp_format_lbl)
  ui_log_timestamp_format_lbl->setText(tr("Log timestamp format:\n") + QDateTime::currentDateTime().toString(Options::options->logTimestampFormat()));

  FROM_UI(QComboBox, log_timestamp_format_combobox)
  registerOption<QComboBox, QString>("log_timestamp_format_combobox", &Options::logTimestampFormat, &Options::setLogTimestampFormat);
  connect(ui_log_timestamp_format_combobox, &QComboBox::currentTextChanged, this, &AOOptionsDialog::on_timestamp_format_edited);

  QString l_current_format = Options::options->logTimestampFormat();

  ui_log_timestamp_format_combobox->setCurrentText(l_current_format);

  if(!Options::options->logTimestampEnabled()) {
    ui_log_timestamp_format_combobox->setDisabled(true);
  }

  FROM_UI(QCheckBox, log_ic_actions_cb)
  registerOption<QCheckBox, bool>("log_ic_actions_cb", &Options::logIcActions, &Options::setLogIcActions);

  FROM_UI(QCheckBox, desync_logs_cb)
  registerOption<QCheckBox, bool>("desync_logs_cb", &Options::desynchronisedLogsEnabled, &Options::setDesynchronisedLogsEnabled);

  FROM_UI(QCheckBox, log_text_cb)
  registerOption<QCheckBox, bool>("log_text_cb", &Options::logToTextFileEnabled, &Options::setLogToTextFileEnabled);

  FROM_UI(QCheckBox, log_demo_cb)
  registerOption<QCheckBox, bool>("log_demo_cb", &Options::logToDemoFileEnabled, &Options::setLogToDemoFileEnabled);

  FROM_UI(QTextBrowser, privacy_policy)
  ui_privacy_policy->setPlainText(tr("Getting privacy policy..."));

  update_values();
}

void AOOptionsDialog::populateAudioDevices()
{
  ui_audio_device_combobox->clear();
  if (needs_default_audiodev()) {
      ui_audio_device_combobox->addItem("default");
  }

  BASS_DEVICEINFO info;
  for (int a = 0; BASS_GetDeviceInfo(a, &info); a++) {
      ui_audio_device_combobox->addItem(info.name);
  }
}

template<>
void AOOptionsDialog::setWidgetData(QCheckBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template<>
bool AOOptionsDialog::widgetData(QCheckBox *widget) const
{
  return widget->isChecked();
}

template<>
void AOOptionsDialog::setWidgetData(QLineEdit *widget, const QString &value)
{
  widget->setText(value);
}

template<>
QString AOOptionsDialog::widgetData(QLineEdit *widget) const
{
  return widget->text();
}

template<>
void AOOptionsDialog::setWidgetData(QLineEdit *widget, const uint16_t &value)
{
  widget->setText(QString::number(value));
}

template<>
uint16_t AOOptionsDialog::widgetData(QLineEdit *widget) const
{
  return widget->text().toUShort();
}

template<>
void AOOptionsDialog::setWidgetData(QPlainTextEdit *widget, const QStringList &value)
{
  widget->setPlainText(value.join('\n'));
}

template<>
QStringList AOOptionsDialog::widgetData(QPlainTextEdit *widget) const
{
  return widget->toPlainText().trimmed().split('\n');
}

template<>
void AOOptionsDialog::setWidgetData(QSpinBox *widget, const int &value)
{
  widget->setValue(value);
}

template<>
int AOOptionsDialog::widgetData(QSpinBox *widget) const
{
  return widget->value();
}

template<>
void AOOptionsDialog::setWidgetData(QComboBox *widget, const QString &value)
{
  for (auto i = 0; i < widget->count(); i++) {
    if (widget->itemText(i) == value) {
      widget->setCurrentIndex(i);
      return;
    }
  }
  qWarning() << "value" << value << "not found for widget" << widget->objectName();
}

template<>
QString AOOptionsDialog::widgetData(QComboBox *widget) const
{
  return widget->currentText();
}

template<>
void AOOptionsDialog::setWidgetData(QGroupBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template<>
bool AOOptionsDialog::widgetData(QGroupBox *widget) const
{
  return widget->isChecked();
}

template<>
void AOOptionsDialog::setWidgetData(QListWidget *widget, const QStringList &value)
{
    widget->addItems(value);
}

template<>
QStringList AOOptionsDialog::widgetData(QListWidget *widget) const
{
    QStringList paths;
    for (auto i =  1; i < widget->count(); i++) {
        paths.append(widget->item(i)->text());
    }
    return paths;
}

template<typename T, typename V>
void AOOptionsDialog::registerOption(const QString &widgetName,
                                     V (Options::*getter)() const,
                                     void (Options::*setter)(V))
{
  auto *widget = findChild<T *>(widgetName);
  if (!widget) {
    qWarning() << "could not find widget" << widgetName;
    return;
  }

  OptionEntry entry;
  entry.load = [=] {
    setWidgetData<T, V>(widget, (options.*getter)());
  };
  entry.save = [=] {
    (options.*setter)(widgetData<T, V>(widget));
  };

  optionEntries.append(entry);
}

void AOOptionsDialog::update_values()
{
    for (const OptionEntry &entry : qAsConst(optionEntries))
      entry.load();

    ao_app->net_manager->request_document(MSDocumentType::PrivacyPolicy, [this](QString document) {
      if (document.isEmpty()) {
        document = tr("Couldn't get the privacy policy.");
      }
      ui_privacy_policy->setHtml(document);
    });
}

void AOOptionsDialog::save_pressed()
{
    for (const OptionEntry &entry : qAsConst(optionEntries))
      entry.save();

    //TODO : Figure out a way to do this proper.
    options.setCallwords(ui_callwords_textbox->toPlainText());
    this->hide();
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
