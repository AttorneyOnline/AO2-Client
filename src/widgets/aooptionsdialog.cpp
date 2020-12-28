#include "widgets/aooptionsdialog.h"
#include "network/hdid.h"
#include "aoapplication.h"
#include "aouiloader.h"
#include "contrib/bass.h"

#include <QPushButton>
#include <QDesktopServices>

AOOptionsDialog::AOOptionsDialog(QWidget *parent, AOApplication *p_ao_app)
  : QDialog(parent), ao_app(p_ao_app)
{
  QUiLoader loader(this);
  QFile uiFile(":/resource/ui/options.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);

  setWindowTitle(tr("Settings"));
  resize(windowWidget->size());

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QDialogButtonBox, dialog_buttons);
  QObject::connect(ui_dialog_buttons, &QDialogButtonBox::accepted,
                   this, &AOOptionsDialog::accept);
  QObject::connect(ui_dialog_buttons, &QDialogButtonBox::rejected,
                   this, &AOOptionsDialog::reject);
  QObject::connect(this, &AOOptionsDialog::accepted,
                   this, &AOOptionsDialog::saveAll);

  // We'll stop updates so that the window won't flicker while it's being made.
  setUpdatesEnabled(false);

  // Fill the combobox with the names of the themes.
  FROM_UI(QComboBox, theme)
  populateThemes();

  // Let's fill out the combobox with the available audio devices.
  FROM_UI(QComboBox, audio_device)
  populateAudioDevices();

  registerOption<QComboBox, QString>("theme", &Options::theme, &Options::setTheme);
  registerOption<QComboBox, QString>("language", &Options::language, &Options::setLanguage);
  registerOption<QLineEdit, QString>("username", &Options::oocName, &Options::setOOCName);
  registerOption<QLineEdit, QString>("ms_hostname", &Options::msAddress, &Options::setMSAddress);
  registerOption<QLineEdit, uint16_t>("ms_port", &Options::msPort, &Options::setMSPort);

  registerOption<QComboBox, QString>("audio_device", &Options::audioDevice, &Options::setAudioDevice);
  registerOption<QSpinBox, int>("blip_rate", &Options::blipRate, &Options::setBlipRate);
  registerOption<QSpinBox, int>("blip_interval", &Options::blipInterval, &Options::setBlipInterval);
  registerOption<QCheckBox, bool>("blank_blips", &Options::blankBlipsEnabled, &Options::setBlankBlipsEnabled);
  registerOption<QCheckBox, bool>("disable_looping_sfx", &Options::disableLoopingSfx, &Options::setDisableLoopingSfx);
  registerOption<QCheckBox, bool>("objection_stops_music",
                                  &Options::objectionStopsMusic, &Options::setObjectionStopsMusic);

  registerOption<QCheckBox, bool>("legacy_scroll", &Options::legacyScrollEnabled, &Options::setLegacyScrollEnabled);
  registerOption<QSpinBox, int>("log_max_lines", &Options::maxLogLines, &Options::setMaxLogLines);
  registerOption<QCheckBox, bool>("log_name_newline", &Options::logNewlineAfterName, &Options::setLogNewlineAfterName);
  registerOption<QSpinBox, int>("log_spacing", &Options::logSpacing, &Options::setLogSpacing);
  registerOption<QCheckBox, bool>("log_timestamps", &Options::logTimestamps, &Options::setLogTimestamps);
  registerOption<QPlainTextEdit, QStringList>("callwords", &Options::callWords, &Options::setCallWords);
  registerOption<QCheckBox, bool>("save_logs", &Options::saveLogs, &Options::setSaveLogs);
  connect(findChild<QPushButton *>("open_log_folder"), &QPushButton::clicked,
          this, &AOOptionsDialog::openLogFolder);

  /*
  registerOption<QButtonGroup, QString>("viewport_scaling_policy", &Options::viewportScalingPolicy,
                                        &Options::setViewportScalingPolicy);
  */

  FROM_UI(QLineEdit, viewport_scale_width);
  FROM_UI(QLineEdit, viewport_scale_height);
  FROM_UI(QLabel, viewport_ratio);
  if (ui_viewport_scale_width && ui_viewport_scale_height && ui_viewport_ratio) {
    optionEntries.append( {
      [=] {
        QSize size = options.viewportScale();
        ui_viewport_scale_width->setText(QString::number(size.width()));
        ui_viewport_scale_height->setText(QString::number(size.height()));
      },
      [=] {
        int w = ui_viewport_scale_width->text().toInt();
        int h = ui_viewport_scale_height->text().toInt();
        if (w <= 100 || h <= 100) {
          qWarning() << "invalid viewport scale" << w << "x" << h;
          w = 256;
          h = 192;
        }
        options.setViewportScale(QSize(w, h));
      }
    });
    connect(ui_viewport_scale_width, &QLineEdit::textEdited,
            this, &AOOptionsDialog::calculateViewportRatio);
    connect(ui_viewport_scale_height, &QLineEdit::textEdited,
            this, &AOOptionsDialog::calculateViewportRatio);
  }

  registerOption<QCheckBox, bool>("enable_discord",
                                  &Options::discordEnabled, &Options::setDiscordEnabled);
  if (auto *hdidWidget = findChild<QLineEdit *>("hdid"))
    hdidWidget->setText(AttorneyOnline::hdid());

  registerOption<QCheckBox, bool>("disable_screenshake",
                                  &Options::disableScreenshake, &Options::setDisableScreenshake);
  registerOption<QCheckBox, bool>("disable_overlay_effects",
                                  &Options::disableOverlayEffects, &Options::setDisableOverlayEffects);
  registerOption<QCheckBox, bool>("disable_frame_sfx",
                                  &Options::disableFrameSfx, &Options::setDisableFrameSfx);
  registerOption<QCheckBox, bool>("disable_log_colors",
                                  &Options::disableLogColors, &Options::setDisableLogColors);
  registerOption<QCheckBox, bool>("disable_custom_chatboxes",
                                  &Options::disableCustomChatboxes, &Options::setDisableCustomChatboxes);
  registerOption<QCheckBox, bool>("disable_custom_shownames",
                                  &Options::disableCustomShownames, &Options::setDisableCustomShownames);
  registerOption<QCheckBox, bool>("sticky_preanim",
                                  &Options::stickyPreanim, &Options::setStickyPreanim);
  registerOption<QCheckBox, bool>("sticky_sfx",
                                  &Options::stickySfx, &Options::setStickySfx);
  registerOption<QCheckBox, bool>("sticky_effects",
                                  &Options::stickyEffects, &Options::setStickyEffects);

  loadAll();

  calculateViewportRatio();

  // When we're done, we should continue the updates!
  setUpdatesEnabled(true);
}

void AOOptionsDialog::populateAudioDevices()
{
  ui_audio_device->clear();
  if (needsDefaultAudioDevice()) {
      ui_audio_device->addItem("default");
  }

  BASS_DEVICEINFO info;
  for (int a = 0; BASS_GetDeviceInfo(a, &info); a++) {
      ui_audio_device->addItem(info.name);
  }
}

void AOOptionsDialog::populateThemes()
{
  ui_theme->clear();
  QDirIterator it(ao_app->get_base_path() + "themes", QDir::Dirs | QDir::NoDotAndDotDot,
                  QDirIterator::NoIteratorFlags);
  while (it.hasNext()) {
    QString actualName = QDir(it.next()).dirName();
    ui_theme->addItem(actualName);
  }
}

static size_t gcd(size_t a, size_t b)
{
  if (a > b)
    std::swap(a, b);
  size_t r = b % a;
  if (r == 0)
    return a;
  else
    return gcd(r, a);
}

void AOOptionsDialog::calculateViewportRatio()
{
  size_t w = ui_viewport_scale_width->text().toInt();
  size_t h = ui_viewport_scale_height->text().toInt();
  if (w <= 0 || h <= 0) {
    ui_viewport_ratio->setText("???");
    return;
  }
  // This will work for the common aspect ratios, which are expressed
  // as integers with no leftover fractional part.
  size_t d = gcd(w, h);
  w /= d;
  h /= d;
  ui_viewport_ratio->setText(QStringLiteral("%1:%2").arg(w).arg(h));
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

void AOOptionsDialog::loadAll()
{
  for (const OptionEntry &entry : optionEntries)
    entry.load();
}

void AOOptionsDialog::saveAll()
{
  for (const OptionEntry &entry : optionEntries)
    entry.save();
}

void AOOptionsDialog::openLogFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(ao_app->get_base_path() + "/logs/"));
}

// TODO: Need to investigate why this exists.
bool AOOptionsDialog::needsDefaultAudioDevice()
{
#if (defined (_WIN32) || defined (_WIN64))
  return true;
#elif (defined (LINUX) || defined (__linux__))
  return false;
#else
#error This operating system is not supported.
#endif
}