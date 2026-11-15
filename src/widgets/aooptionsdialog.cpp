#include "widgets/aooptionsdialog.h"
#include "QDesktopServices"
#include "aoapplication.h"
#include "bass.h"
#include "file_functions.h"
#include "mount_manager.h"
#include "networkmanager.h"
#include "options.h"
#include "package_metadata.h"

#include <QCheckBox>
#include <QCollator>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPixmap>
#include <QPushButton>
#include <QResource>
#include <QScrollBar>
#include <QSet>
#include <QSpinBox>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QTextDocument>
#include <QtMath>
#include <QUiLoader>
#include <QVBoxLayout>
#include <QRadioButton>

namespace {
constexpr int kMountItemMargin = 6;
constexpr int kMountIconSize = 64;
constexpr int kMountIconSpacing = 10;

QString plainTextToHtml(const QString &text)
{
  QString escaped = text.toHtmlEscaped();
  escaped.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
  escaped.replace(QLatin1Char('\r'), QLatin1Char('\n'));
  escaped.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
  return escaped;
}

QSize mountIconExtent(const QStyleOptionViewItem &option)
{
  QSize extent = option.decorationSize;
  extent.setWidth(qMax(extent.width(), kMountIconSize));
  extent.setHeight(qMax(extent.height(), kMountIconSize));
  return extent;
}

class MountItemDelegate : public QStyledItemDelegate {
public:
  explicit MountItemDelegate(QObject *parent = nullptr)
      : QStyledItemDelegate(parent)
  {
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override
  {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QString html = opt.text;
    const QIcon icon = opt.icon;

    // Let the style paint the row background, focus and selection. The rich
    // text and icon are drawn below so we can control their layout.
    opt.text.clear();
    opt.icon = QIcon();
    QStyle *style = opt.widget != nullptr ? opt.widget->style()
                                          : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    const QSize extent = mountIconExtent(opt);
    const bool hasIcon = !icon.isNull();
    QRect iconRect(opt.rect.left() + kMountItemMargin,
                   opt.rect.top() + kMountItemMargin, extent.width(),
                   extent.height());

    if (hasIcon) {
      const qreal dpr =
          painter->device() != nullptr ? painter->device()->devicePixelRatioF()
                                       : 1.0;
      const QSize pixelSize(qRound(extent.width() * dpr),
                            qRound(extent.height() * dpr));
      const QPixmap source = icon.pixmap(pixelSize);
      if (!source.isNull()) {
        QPixmap scaled =
            source.scaled(pixelSize, Qt::KeepAspectRatio,
                          Qt::SmoothTransformation);
        scaled.setDevicePixelRatio(dpr);
        QRect target(iconRect.topLeft(),
                     QSize(qRound(scaled.width() / dpr),
                           qRound(scaled.height() / dpr)));
        target.moveCenter(iconRect.center());
        painter->drawPixmap(target, scaled);
      }
    }

    const int textLeft = opt.rect.left() + kMountItemMargin +
                         (hasIcon ? extent.width() + kMountIconSpacing : 0);
    const int textWidth = opt.rect.right() - kMountItemMargin - textLeft + 1;
    if (textWidth <= 0 || html.isEmpty()) {
      return;
    }

    const bool selected = (opt.state & QStyle::State_Selected) != 0;
    QPalette::ColorGroup colorGroup = QPalette::Active;
    if ((opt.state & QStyle::State_Enabled) == 0) {
      colorGroup = QPalette::Disabled;
    }
    else if ((opt.state & QStyle::State_Active) == 0) {
      colorGroup = QPalette::Inactive;
    }
    const QColor textColor =
        opt.palette.color(colorGroup, selected ? QPalette::HighlightedText
                                               : QPalette::Text);

    QTextDocument document;
    document.setDocumentMargin(0);
    document.setDefaultFont(opt.font);
    document.setDefaultStyleSheet(
        QStringLiteral("body { color: %1; }").arg(textColor.name()));
    document.setHtml(html);
    document.setTextWidth(textWidth);

    const int textHeight = qCeil(document.size().height());
    int textTop = opt.rect.top() + kMountItemMargin;
    if (hasIcon && textHeight < extent.height()) {
      textTop += (extent.height() - textHeight) / 2;
    }

    painter->save();
    painter->setClipRect(opt.rect);
    painter->translate(textLeft, textTop);
    document.drawContents(painter);
    painter->restore();
  }

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override
  {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QSize extent = mountIconExtent(opt);
    const bool hasIcon = !opt.icon.isNull();

    int width = opt.rect.width();
    if (width <= 0 && opt.widget != nullptr) {
      width = opt.widget->width();
      if (const QAbstractItemView *view =
              qobject_cast<const QAbstractItemView *>(opt.widget)) {
        if (view->verticalScrollBar() != nullptr &&
            view->verticalScrollBar()->isVisible()) {
          width -= view->verticalScrollBar()->width();
        }
      }
    }
    if (width <= 0) {
      width = 480;
    }

    const int textLeft =
        kMountItemMargin + (hasIcon ? extent.width() + kMountIconSpacing : 0);
    const int textWidth = qMax(1, width - textLeft - kMountItemMargin);

    QTextDocument document;
    document.setDocumentMargin(0);
    document.setDefaultFont(opt.font);
    document.setHtml(opt.text);
    document.setTextWidth(textWidth);
    const int textHeight = qCeil(document.size().height());

    const int contentHeight =
        hasIcon ? qMax(extent.height(), textHeight)
                : qMax(textHeight, QFontMetrics(opt.font).height());
    return QSize(width, contentHeight + 2 * kMountItemMargin);
  }
};
} // namespace

#define FROM_UI(type, name)                                                    \
  ;                                                                            \
  ui_##name = findChild<type *>(#name);

AOOptionsDialog::AOOptionsDialog(QDialog *parent, AOApplication *p_ao_app, int initial_tab)
    : QDialog(parent)
{
  ao_app = p_ao_app;
  setupUI();
  if (ao_app != nullptr && ao_app->mounts() != nullptr) {
    connect(ao_app->mounts(), &MountManager::changed, this,
            &AOOptionsDialog::onMountsChanged);
  }
  ui_settings_tabs->setCurrentIndex(initial_tab);
}

void AOOptionsDialog::populateAudioDevices()
{
  ui_audio_device_combobox->clear();
  if (needsDefaultAudioDevice()) {
    ui_audio_device_combobox->addItem("default");
  }

  BASS_DEVICEINFO info;
  for (int a = 0; BASS_GetDeviceInfo(a, &info); a++) {
    ui_audio_device_combobox->addItem(info.name);
  }
}

template <>
void AOOptionsDialog::setWidgetData(QCheckBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template <> bool AOOptionsDialog::widgetData(QCheckBox *widget) const
{
  return widget->isChecked();
}

template <>
void AOOptionsDialog::setWidgetData(QLineEdit *widget, const QString &value)
{
  widget->setText(value);
}

template <> QString AOOptionsDialog::widgetData(QLineEdit *widget) const
{
  return widget->text();
}

template <>
void AOOptionsDialog::setWidgetData(QLineEdit *widget, const uint16_t &value)
{
  widget->setText(QString::number(value));
}

template <> uint16_t AOOptionsDialog::widgetData(QLineEdit *widget) const
{
  return widget->text().toUShort();
}

template <>
void AOOptionsDialog::setWidgetData(QPlainTextEdit *widget,
                                    const QStringList &value)
{
  widget->setPlainText(value.join('\n'));
}

template <>
QStringList AOOptionsDialog::widgetData(QPlainTextEdit *widget) const
{
  return widget->toPlainText().trimmed().split('\n');
}

template <>
void AOOptionsDialog::setWidgetData(QSpinBox *widget, const int &value)
{
  widget->setValue(value);
}

template <> int AOOptionsDialog::widgetData(QSpinBox *widget) const
{
  return widget->value();
}

template <>
void AOOptionsDialog::setWidgetData(QDoubleSpinBox *widget, const double &value)
{
  widget->setValue(value);
}

template <> double AOOptionsDialog::widgetData(QDoubleSpinBox *widget) const
{
  return widget->value();
}

template <>
void AOOptionsDialog::setWidgetData(QComboBox *widget, const QString &value)
{
  for (auto i = 0; i < widget->count(); i++) {
    if (widget->itemText(i) == value) {
      widget->setCurrentIndex(i);
      return;
    }
  }
  qWarning() << "value" << value << "not found for widget"
             << widget->objectName();
}

template <> QString AOOptionsDialog::widgetData(QComboBox *widget) const
{
  return widget->currentText();
}

template <>
void AOOptionsDialog::setWidgetData(QGroupBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template <> bool AOOptionsDialog::widgetData(QGroupBox *widget) const
{
  return widget->isChecked();
}

template <>
void AOOptionsDialog::setWidgetData(QListWidget *widget,
                                    const QStringList &value)
{
  widget->addItems(value);
}

template <> QStringList AOOptionsDialog::widgetData(QListWidget *widget) const
{
  QStringList paths;
  for (auto i = 1; i < widget->count(); i++) {
    paths.append(widget->item(i)->text());
  }
  return paths;
}

template <>
void AOOptionsDialog::setWidgetData(QRadioButton *widget, const bool &value)
{
  widget->toggled(value);
}

template <> bool AOOptionsDialog::widgetData(QRadioButton *widget) const
{
  return widget->isChecked();
}

// Modificar la función widgetData
template <> TableData AOOptionsDialog::widgetData(QTableWidget *widget) const
{
    TableData tableData;
    QSet<QString> uniqueURLs; // Conjunto para almacenar URLs únicas

    // Obtener las cabeceras (nombres de las filas) de la tabla
    for (int row = 0; row < widget->rowCount(); ++row) {
        QTableWidgetItem* headerItem = widget->verticalHeaderItem(row);
        QString headerData = headerItem ? headerItem->text() : "";
        tableData.headers.append(headerData);
    }

    // Obtener los datos de la única columna de la tabla
    for (int row = 0; row < widget->rowCount(); ++row) {
        QTableWidgetItem* item = widget->item(row, 0);
        QString rowData = item ? item->text() : "";
        // Verificar si la URL ya ha sido agregada previamente
        if (!rowData.isEmpty() && !uniqueURLs.contains(rowData)) {
            QStringList rowDataList; // Crear una lista de un solo elemento
            rowDataList.append(rowData); // Agregar el elemento a la lista
            tableData.rows.append(rowDataList); // Agregar la lista a la lista de filas
            uniqueURLs.insert(rowData); // Agregar la URL al conjunto
        }
    }

    return tableData;
}

// Modificar la función setWidgetData
template <>
void AOOptionsDialog::setWidgetData(QTableWidget *widget, const TableData &data)
{
    widget->setRowCount(data.headers.size());
    for (int row = 0; row < data.headers.size(); ++row) {
        QString headerData = data.headers.value(row);
        QTableWidgetItem* headerItem = new QTableWidgetItem(headerData);
        widget->setVerticalHeaderItem(row, headerItem);
        widget->verticalHeaderItem(row)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    widget->setColumnCount(1);
    for (int row = 0; row < data.rows.size(); ++row) {
        const QStringList &rowData = data.rows.at(row);
        QString joinedRowData = rowData.join(", ");
        QTableWidgetItem* item = new QTableWidgetItem(joinedRowData);
        widget->setItem(row, 0, item);
    }
}

template <typename T, typename V>
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
    setWidgetData<T, V>(widget, (Options::getInstance().*getter)());
  };
  entry.save = [=] {
    (Options::getInstance().*setter)(widgetData<T, V>(widget));
  };

  optionEntries.append(entry);
}

void AOOptionsDialog::updateValues()
{
  QStringList themes;
  if (ao_app != nullptr)
    themes = ao_app->availableThemes();

  // Resort list to match numeric sorting found in Windows.
  QCollator l_sorting;
  l_sorting.setNumericMode(true);
  std::sort(themes.begin(), themes.end(), l_sorting);

  for (const QString &l_theme : std::as_const(themes)) {
    ui_theme_combobox->addItem(l_theme);
  }

  QStringList l_subthemes =
      QDir(ao_app->get_real_path(ao_app->get_theme_path("")))
          .entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &l_subtheme : std::as_const(l_subthemes)) {
    if (l_subtheme.toLower() != "server" && l_subtheme.toLower() != "default" &&
        l_subtheme.toLower() != "effects" && l_subtheme.toLower() != "misc") {
      ui_subtheme_combobox->addItem(l_subtheme);
    }
  }

  ao_app->net_manager->request_document(
      MSDocumentType::PrivacyPolicy, [this](QString document) {
        if (document.isEmpty()) {
          document = tr("Couldn't get the privacy policy.");
        }
        ui_privacy_policy->setHtml(document);
      });

  for (const OptionEntry &entry : std::as_const(optionEntries)) {
    entry.load();
  }
}

void AOOptionsDialog::savePressed()
{
  processPendingPackageRemovals();

  bool l_reload_theme_required =
      (ui_theme_combobox->currentText() != Options::getInstance().theme()) ||
      (ui_theme_scaling_factor_sb->value() !=
       Options::getInstance().themeScalingFactor());
  for (const OptionEntry &entry : std::as_const(optionEntries)) {
    entry.save();
  }

  if (asset_cache_dirty && ao_app != nullptr)
    ao_app->invalidate_lookup_cache();
  asset_cache_dirty = false;

  if (l_reload_theme_required) {
    emit reloadThemeRequest();
  }
  close();
}

void AOOptionsDialog::discardPressed()
{
  m_pendingPackageRemovals.clear();
  close();
}

void AOOptionsDialog::buttonClicked(QAbstractButton *button)
{
  if (ui_settings_buttons->buttonRole(button) == QDialogButtonBox::ResetRole) {
    if (QMessageBox::question(
            this, "", "Restore default settings?\nThis can't be undone!",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
      // Destructive operation.
      Options::getInstance().clearConfig();
      updateValues();
    }
  }
}

void AOOptionsDialog::onReloadThemeClicked()
{
  Options::getInstance().setTheme(ui_theme_combobox->currentText());
  Options::getInstance().setSettingsSubTheme(ui_subtheme_combobox->currentText());
  Options::getInstance().setAnimatedThemeEnabled(
      ui_animated_theme_cb->isChecked());
  emit reloadThemeRequest();
  delete layout();
  delete ui_settings_widget;
  optionEntries.clear();
  setupUI();
}

void AOOptionsDialog::themeChanged(int i)
{
  ui_subtheme_combobox->clear();
  // Fill the combobox with the names of the themes.
  ui_subtheme_combobox->addItem("server");
  ui_subtheme_combobox->addItem("default");

  QStringList l_subthemes = QDir(ao_app->get_real_path(ao_app->get_theme_path(
                                     "", ui_theme_combobox->itemText(i))))
                                .entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  for (const QString &l_subthemes : std::as_const(l_subthemes)) {
    if (l_subthemes.toLower() != "server" &&
        l_subthemes.toLower() != "default" &&
        l_subthemes.toLower() != "effects" && l_subthemes.toLower() != "misc") {
      ui_subtheme_combobox->addItem(l_subthemes);
    }
  }

  QString l_ressource_name = Options::getInstance().theme() + ".rcc";
  QString l_resource =
      ao_app->get_asset("themes/" + ui_theme_combobox->currentText() + ".rcc");
  if (l_resource.isEmpty()) {
    QResource::unregisterResource(
        ao_app->get_asset("themes/" + l_ressource_name));
    qDebug() << "Unable to locate ressource file" << l_ressource_name;
    return;
  }
  QResource::registerResource(l_resource);
}

void AOOptionsDialog::setupUI()
{
  QUiLoader l_loader(this);
  QFile l_uiFile(Options::getInstance().getUIAsset("options_dialog.ui"));
  if (!l_uiFile.open(QFile::ReadOnly)) {
    qWarning() << "Unable to open file " << l_uiFile.fileName();
    return;
  }

  ui_settings_widget = l_loader.load(&l_uiFile, this);

  auto l_layout = new QVBoxLayout(this);
  l_layout->addWidget(ui_settings_widget);

  // General dialog element.
  FROM_UI(QDialogButtonBox, settings_buttons);

  connect(ui_settings_buttons, &QDialogButtonBox::accepted, this,
          &AOOptionsDialog::savePressed);
  connect(ui_settings_buttons, &QDialogButtonBox::rejected, this,
          &AOOptionsDialog::discardPressed);
  connect(ui_settings_buttons, &QDialogButtonBox::clicked, this,
          &AOOptionsDialog::buttonClicked);

  // Gameplay Tab
  FROM_UI(QComboBox, theme_combobox)
  connect(ui_theme_combobox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &AOOptionsDialog::themeChanged);

  registerOption<QComboBox, QString>("theme_combobox", &Options::theme,
                                     &Options::setTheme);

  FROM_UI(QComboBox, subtheme_combobox)
  registerOption<QComboBox, QString>("subtheme_combobox", &Options::settingsSubTheme,
                                     &Options::setSettingsSubTheme);

  FROM_UI(QPushButton, theme_reload_button)
  connect(ui_theme_reload_button, &QPushButton::clicked, this,
          &::AOOptionsDialog::onReloadThemeClicked);

  FROM_UI(QPushButton, theme_folder_button)
  connect(ui_theme_folder_button, &QPushButton::clicked, this, [=] {
    QString p_path = ao_app->get_real_path(ao_app->get_theme_path(
        "", ui_theme_combobox->itemText(ui_theme_combobox->currentIndex())));
    if (!dir_exists(p_path)) {
      return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
  });

  FROM_UI(QDoubleSpinBox, theme_scaling_factor_sb)
  FROM_UI(QCheckBox, animated_theme_cb)
  FROM_UI(QSpinBox, stay_time_spinbox)
  FROM_UI(QCheckBox, instant_objection_cb)
  FROM_UI(QSpinBox, text_crawl_spinbox)
  FROM_UI(QSpinBox, chat_ratelimit_spinbox)
  FROM_UI(QLineEdit, username_textbox)
  FROM_UI(QCheckBox, showname_cb)
  FROM_UI(QLineEdit, default_showname_textbox)
  FROM_UI(QLineEdit, ms_textbox)
  FROM_UI(QCheckBox, discord_cb)
  FROM_UI(QComboBox, language_combobox)
  FROM_UI(QComboBox, scaling_combobox)
  FROM_UI(QCheckBox, shake_cb)
  FROM_UI(QCheckBox, effects_cb)
  FROM_UI(QCheckBox, framenetwork_cb)
  FROM_UI(QCheckBox, colorlog_cb)
  FROM_UI(QCheckBox, stickysounds_cb)
  FROM_UI(QCheckBox, stickyeffects_cb)
  FROM_UI(QCheckBox, stickypres_cb)
  FROM_UI(QCheckBox, customchat_cb)
  FROM_UI(QCheckBox, sticker_cb)
  FROM_UI(QCheckBox, continuous_cb)
  FROM_UI(QCheckBox, category_stop_cb)
  FROM_UI(QCheckBox, sfx_on_idle_cb)
  FROM_UI(QCheckBox, evidence_double_click_cb)
  FROM_UI(QLineEdit, playerlist_format_edit)

  FROM_UI(QCheckBox, crossfade_cb)
  FROM_UI(QCheckBox, hide_typing_cb)
  FROM_UI(QCheckBox, stop_typing_cb)
  FROM_UI(QCheckBox, menu_bar_visible_btn_cb)
  FROM_UI(QCheckBox, asset_streaming_cb)
  FROM_UI(QCheckBox, image_streaming_cb)

  registerOption<QDoubleSpinBox, double>("theme_scaling_factor_sb",
                                &Options::themeScalingFactor,
                                &Options::setThemeScalingFactor);
  registerOption<QCheckBox, bool>("animated_theme_cb",
                                  &Options::animatedThemeEnabled,
                                  &Options::setAnimatedThemeEnabled);
  registerOption<QSpinBox, int>("stay_time_spinbox", &Options::textStayTime,
                                &Options::setTextStayTime);
  registerOption<QCheckBox, bool>("instant_objection_cb",
                                  &Options::objectionSkipQueueEnabled,
                                  &Options::setObjectionSkipQueueEnabled);
  registerOption<QSpinBox, int>("text_crawl_spinbox", &Options::textCrawlSpeed,
                                &Options::setTextCrawlSpeed);
  registerOption<QSpinBox, int>("chat_ratelimit_spinbox",
                                &Options::chatRateLimit,
                                &Options::setChatRateLimit);
  registerOption<QLineEdit, QString>("username_textbox", &Options::username,
                                     &Options::setUsername);
  registerOption<QCheckBox, bool>("showname_cb",
                                  &Options::customShownameEnabled,
                                  &Options::setCustomShownameEnabled);
  registerOption<QLineEdit, QString>("default_showname_textbox",
                                     &Options::shownameOnJoin,
                                     &Options::setShownameOnJoin);
  registerOption<QLineEdit, QString>("ms_textbox",
                                     &Options::alternativeMasterserver,
                                     &Options::setAlternativeMasterserver);
  registerOption<QCheckBox, bool>("discord_cb", &Options::discordEnabled,
                                  &Options::setDiscordEnabled);
  registerOption<QComboBox, QString>("language_combobox", &Options::language,
                                     &Options::setLanguage);
  registerOption<QComboBox, QString>("scaling_combobox",
                                     &Options::defaultScalingMode,
                                     &Options::setDefaultScalingMode);

  // Populate scaling dropdown. This is necessary as we need the user data
  // embeeded into the entry.
  ui_scaling_combobox->addItem("pixel", "pixel");
  ui_scaling_combobox->addItem("smooth", "smooth");

  registerOption<QCheckBox, bool>("shake_cb", &Options::shakeEnabled,
                                  &Options::setShakeEnabled);
  registerOption<QCheckBox, bool>("effects_cb", &Options::effectsEnabled,
                                  &Options::setEffectsEnabled);
  registerOption<QCheckBox, bool>("framenetwork_cb",
                                  &Options::networkedFrameSfxEnabled,
                                  &Options::setNetworkedFrameSfxEnabled);
  registerOption<QCheckBox, bool>("colorlog_cb", &Options::colorLogEnabled,
                                  &Options::setColorLogEnabled);
  registerOption<QCheckBox, bool>(
      "stickysounds_cb", &Options::clearSoundsDropdownOnPlayEnabled,
      &Options::setClearSoundsDropdownOnPlayEnabled);
  registerOption<QCheckBox, bool>(
      "stickyeffects_cb", &Options::clearEffectsDropdownOnPlayEnabled,
      &Options::setClearEffectsDropdownOnPlayEnabled);
  registerOption<QCheckBox, bool>("stickypres_cb",
                                  &Options::clearPreOnPlayEnabled,
                                  &Options::setClearPreOnPlayEnabled);
  registerOption<QCheckBox, bool>("customchat_cb",
                                  &Options::customChatboxEnabled,
                                  &Options::setCustomChatboxEnabled);
  registerOption<QCheckBox, bool>("sticker_cb",
                                  &Options::characterStickerEnabled,
                                  &Options::setCharacterStickerEnabled);
  registerOption<QCheckBox, bool>("continuous_cb",
                                  &Options::continuousPlaybackEnabled,
                                  &Options::setContinuousPlaybackEnabled);
  registerOption<QCheckBox, bool>("category_stop_cb",
                                  &Options::stopMusicOnCategoryEnabled,
                                  &Options::setStopMusicOnCategoryEnabled);
  registerOption<QCheckBox, bool>("sfx_on_idle_cb",
                                  &Options::playSelectedSFXOnIdle,
                                  &Options::setPlaySelectedSFXOnIdle);
  registerOption<QCheckBox, bool>("evidence_double_click_cb",
                                  &Options::evidenceDoubleClickEdit,
                                  &Options::setEvidenceDoubleClickEdit);
  registerOption<QLineEdit, QString>("playerlist_format_edit",
                                     &Options::playerlistFormatString,
                                     &Options::setPlayerlistFormatString);

  registerOption<QCheckBox, bool>("hide_typing_cb",
                                  &Options::hideTyping,
                                  &Options::setHideTyping);
  registerOption<QCheckBox, bool>("stop_typing_cb",
                                  &Options::stopTypingIcon,
                                  &Options::setStopTypingIcon);
  registerOption<QCheckBox, bool>("crossfade_cb",
                                  &Options::crossfade,
                                  &Options::setCrossfade);
  registerOption<QCheckBox, bool>("menu_bar_visible_btn_cb",
                                  &Options::menuBarLocked,
                                  &Options::setMenuBarLocked);
  registerOption<QCheckBox, bool>("auto_updates_cb",
                                  &Options::autoUpdates,
                                  &Options::setAutoUpdates);
  registerOption<QCheckBox, bool>("image_streaming_cb",
                                  &Options::imageStreaming,
                                  &Options::setImageStreaming);
  registerOption<QCheckBox, bool>("asset_streaming_cb",
                                  &Options::assetStreaming,
                                  &Options::setAssetStreaming);

  // Callwords tab. This could just be a QLineEdit, but no, we decided to allow
  // people to put a billion entries in.
  // -> Let people add whatever they want, honestly
  FROM_UI(QPlainTextEdit, callwords_textbox)
  FROM_UI(QRadioButton, callwords_whole_word)
  FROM_UI(QRadioButton, callwords_case_sensitive)

  registerOption<QPlainTextEdit, QStringList>("callwords_textbox", 
                                  &Options::callwords, 
                                  &Options::setCallwords);
  registerOption<QRadioButton, bool>("callwords_whole_word",
                                  &Options::callwords_WholeWord,
                                  &Options::setCallwords_WholeWord);
  registerOption<QRadioButton, bool>("callwords_case_sensitive",
                                  &Options::callwords_CaseSensitive,
                                  &Options::setCallwords_CaseSensitive);

  if (Options::getInstance().callwords_WholeWord()) {
    ui_callwords_whole_word->setChecked(true);
  }
  
  if (Options::getInstance().callwords_CaseSensitive()) {
    ui_callwords_case_sensitive->setChecked(true);
  }

  FROM_UI(QPlainTextEdit, blacklist_textbox)
  FROM_UI(QRadioButton, blacklist_whole_word)
  FROM_UI(QRadioButton, blacklist_case_sensitive)
  FROM_UI(QLineEdit, replace_with_textbox)
  
  registerOption<QPlainTextEdit, QStringList>("blacklist_textbox",
                                  &Options::filteredWords, 
                                  &Options::setFilteredWords);
  registerOption<QRadioButton, bool>("blacklist_whole_word", 
                                  &Options::filteredWords_WholeWord, 
                                  &Options::setFilteredWords_WholeWord);
  registerOption<QRadioButton, bool>("blacklist_case_sensitive", 
                                  &Options::filteredWords_CaseSensitive, 
                                  &Options::setFilteredWords_CaseSensitive);
  registerOption<QLineEdit, QString>("replace_with_textbox", 
                                  &Options::filteredWords_ReplacedCharacter, 
                                  &Options::setFilteredWords_ReplacedCharacter);

  if (Options::getInstance().filteredWords_WholeWord()) {
    ui_blacklist_whole_word->setChecked(true);
  }

  if (Options::getInstance().filteredWords_CaseSensitive()) {
    ui_blacklist_case_sensitive->setChecked(true);
  }

  // Audio tab.
  FROM_UI(QComboBox, audio_device_combobox)
  populateAudioDevices();
  registerOption<QComboBox, QString>("audio_device_combobox",
                                     &Options::audioOutputDevice,
                                     &Options::setAudioOutputDevice);

  FROM_UI(QSpinBox, suppress_audio_spinbox)
  FROM_UI(QSpinBox, callwords_audio_spinbox)
  FROM_UI(QSpinBox, modcalls_audio_spinbox)
  FROM_UI(QSpinBox, bliprate_spinbox)
  FROM_UI(QCheckBox, blank_blips_cb)
  FROM_UI(QCheckBox, loopsfx_cb)
  FROM_UI(QCheckBox, objectmusic_cb)
  FROM_UI(QCheckBox, disablestreams_cb)

  registerOption<QSpinBox, int>("suppress_audio_spinbox",
                                &Options::defaultSuppressAudio,
                                &Options::setDefaultSupressedAudio);
  registerOption<QSpinBox, int>("callwords_audio_spinbox",
                                &Options::callwordsVolume,
                                &Options::setCallwordsVolume);
  registerOption<QSpinBox, int>("modcalls_audio_spinbox",
                                &Options::modcallsVolume,
                                &Options::setModcallsVolume);
  registerOption<QSpinBox, int>("bliprate_spinbox", &::Options::blipRate,
                                &Options::setBlipRate);
  registerOption<QCheckBox, bool>("blank_blips_cb", &Options::blankBlip,
                                  &Options::setBlankBlip);
  registerOption<QCheckBox, bool>("loopsfx_cb", &Options::loopingSfx,
                                  &Options::setLoopingSfx);
  registerOption<QCheckBox, bool>("objectmusic_cb",
                                  &Options::objectionStopMusic,
                                  &Options::setObjectionStopMusic);
  registerOption<QCheckBox, bool>("disablestreams_cb",
                                  &Options::streamingEnabled,
                                  &Options::setStreamingEnabled);

  // Asset tab
  FROM_UI(QListWidget, mount_list)
  ui_mount_list->setItemDelegate(new MountItemDelegate(ui_mount_list));
  ui_mount_list->setIconSize(QSize(kMountIconSize, kMountIconSize));
  ui_mount_list->setWordWrap(true);
  ui_mount_list->setUniformItemSizes(false);
  ui_mount_list->setResizeMode(QListView::Adjust);
  OptionEntry mountEntry;
  mountEntry.load = [this] { populateMountList(); };
  mountEntry.save = [this] { saveMountList(); };
  optionEntries.append(mountEntry);

  FROM_UI(QPushButton, mount_add)
  connect(ui_mount_add, &QPushButton::clicked, this, [this] {
    QString path = QFileDialog::getExistingDirectory(
        this, tr("Select a base folder or package"), get_app_dir_path(),
        QFileDialog::ShowDirsOnly);
    if (path.isEmpty()) {
      return;
    }

    MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
    if (manager == nullptr) {
      return;
    }

    QString entry = manager->entryForPath(path);
    if (entry.isEmpty() || manager->containsEntry(entry) ||
        mountEntryIsListed(entry)) {
      return;
    }

    // Folders outside packages/ can either stay where they are or be moved
    // into the packages folder so the client discovers them automatically.
    if (!manager->isPackageEntry(entry)) {
      QMessageBox box(this);
      box.setIcon(QMessageBox::Question);
      box.setWindowTitle(tr("Add asset folder"));
      box.setText(tr("Do you want to move this folder into your packages folder?"));
      box.setInformativeText(
          tr("Add as external: the folder won't be moved and the client will reference it from the outside. "
             "Packages live only within the client folder.\n"
             "Move into packages: move the entire folder into your packages folder. "
             "Please make sure you actually selected a valid package folder.\n"
             "(NOTE: 'move into packages' is deliberately disabled in this beta)")
          );

      auto *externalButton =
          box.addButton(tr("Add as external"), QMessageBox::ActionRole);
      auto *moveButton =
          box.addButton(tr("Move into packages"), QMessageBox::ActionRole);
      auto *cancelButton = box.addButton(QMessageBox::Cancel);
      // disable for now this is like absurdly dangerous
      // this should only happen for zip files honestly, full folder copy is crazy
      moveButton->setDisabled(true);
      box.setDefaultButton(externalButton);
      box.setEscapeButton(cancelButton);
      box.exec();

      if (box.clickedButton() == cancelButton) {
        return;
      }

      if (box.clickedButton() == moveButton) {
        // early return for safety. Function deliberately not implemented yet.
        return;

        const QString destPath = QDir(manager->packagesPath())
                                     .filePath(QFileInfo(path).fileName());
        QString error;
        if (!moveDirectoryInto(path, destPath, &error)) {
          QMessageBox::warning(this, tr("Couldn't move folder"), error);
          return;
        }

        entry = manager->entryForPath(destPath);
        if (entry.isEmpty()) {
          return;
        }
      }
    }

    addMountItem(entry);
    ui_mount_list->setCurrentRow(ui_mount_list->count() - 1);

    // quick hack to update buttons
    emit ui_mount_list->itemSelectionChanged();
    asset_cache_dirty = true;
  });

  FROM_UI(QPushButton, mount_remove)
  connect(ui_mount_remove, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty()) return;

    QListWidgetItem *item = selected[0];
    if (ui_mount_list->row(item) <= 0) return;

    MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
    const QString entry = item->data(Qt::UserRole).toString();
    if (manager != nullptr && manager->isPackageEntry(entry)) {
      QMessageBox box(this);
      box.setIcon(QMessageBox::Warning);
      box.setWindowTitle(tr("Remove package"));
      box.setText(tr("Remove package \"%1\"?").arg(entry));
      box.setInformativeText(
          tr("The package folder will be moved to the system Trash when you "
             "press Save.\n\nIf the Trash isn't available, you'll be warned "
             "before anything is permanently deleted."));

      auto *removeButton =
          box.addButton(tr("Remove"), QMessageBox::DestructiveRole);
      auto *cancelButton = box.addButton(QMessageBox::Cancel);
      box.setDefaultButton(cancelButton);
      box.setEscapeButton(cancelButton);
      box.exec();

      if (box.clickedButton() != removeButton) {
        return;
      }

      m_pendingPackageRemovals.insert(entry);
    }

    delete item;
    emit ui_mount_list->itemSelectionChanged();
    asset_cache_dirty = true;
  });

  FROM_UI(QPushButton, mount_up)
  connect(ui_mount_up, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty()) return;
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    if (row <= 0) return;
    ui_mount_list->takeItem(row);
    int new_row = qMax(1, row - 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  FROM_UI(QPushButton, mount_down)
  connect(ui_mount_down, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty()) return;
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    if (row <= 0) return;
    ui_mount_list->takeItem(row);
    int new_row = qMin(ui_mount_list->count(), row + 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  FROM_UI(QPushButton, mount_clear_cache)
  connect(ui_mount_clear_cache, &QPushButton::clicked, this, [this] {
    asset_cache_dirty = true;
    ui_mount_clear_cache->setEnabled(false);
  });

  FROM_UI(QPushButton, mount_open_folder)
  connect(ui_mount_open_folder, &QPushButton::clicked, this, [this] {
    QString path;
    auto selected = ui_mount_list->selectedItems();
    if (!selected.isEmpty() && ui_mount_list->row(selected[0]) > 0) {
      path = mountEntryPath(selected[0]->data(Qt::UserRole).toString());
    }
    if (path.isEmpty()) {
      path = get_base_path();
    }
    if (!dir_exists(path)) {
      return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
  });

  connect(ui_mount_list, &QListWidget::itemSelectionChanged, this, [this] {
    auto selected_items = ui_mount_list->selectedItems();
    bool row_selected = !selected_items.isEmpty() &&
                        ui_mount_list->row(selected_items[0]) > 0;
    MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
    ui_mount_remove->setEnabled(row_selected && manager != nullptr);
    ui_mount_up->setEnabled(row_selected);
    ui_mount_down->setEnabled(row_selected);

    if (!row_selected) return;

    int row = ui_mount_list->row(selected_items[0]);
    if (row <= 1) ui_mount_up->setEnabled(false);
    if (row >= ui_mount_list->count() - 1) ui_mount_down->setEnabled(false);
  });

  // Logging tab
  FROM_UI(QCheckBox, downwards_cb)
  FROM_UI(QSpinBox, length_spinbox)
  FROM_UI(QCheckBox, log_newline_cb)
  FROM_UI(QSpinBox, log_margin_spinbox)
  FROM_UI(QLabel, log_timestamp_format_lbl)
  FROM_UI(QComboBox, log_timestamp_format_combobox)

  registerOption<QCheckBox, bool>("downwards_cb",
                                  &Options::logDirectionDownwards,
                                  &Options::setLogDirectionDownwards);
  registerOption<QSpinBox, int>("length_spinbox", &Options::maxLogSize,
                                &Options::setMaxLogSize);
  registerOption<QCheckBox, bool>("log_newline_cb", &Options::logNewline,
                                  &Options::setLogNewline);
  registerOption<QSpinBox, int>("log_margin_spinbox", &Options::logMargin,
                                &Options::setLogMargin);

  FROM_UI(QCheckBox, log_timestamp_cb)
  registerOption<QCheckBox, bool>("log_timestamp_cb",
                                  &Options::logTimestampEnabled,
                                  &Options::setLogTimestampEnabled);
  connect(ui_log_timestamp_cb, &QCheckBox::stateChanged, this,
          &::AOOptionsDialog::timestampCbChanged);
  ui_log_timestamp_format_lbl->setText(
      tr("Log timestamp format:\n") +
      QDateTime::currentDateTime().toString(
          Options::getInstance().logTimestampFormat()));

  FROM_UI(QComboBox, log_timestamp_format_combobox)
  registerOption<QComboBox, QString>("log_timestamp_format_combobox",
                                     &Options::logTimestampFormat,
                                     &Options::setLogTimestampFormat);
  connect(ui_log_timestamp_format_combobox, &QComboBox::currentTextChanged,
          this, &::AOOptionsDialog::onTimestampFormatEdited);

  QString l_current_format = Options::getInstance().logTimestampFormat();

  ui_log_timestamp_format_combobox->setCurrentText(l_current_format);

  if (!Options::getInstance().logTimestampEnabled()) {
    ui_log_timestamp_format_combobox->setDisabled(true);
  }

  FROM_UI(QCheckBox, log_ic_actions_cb)
  FROM_UI(QCheckBox, desync_logs_cb)
  FROM_UI(QCheckBox, log_text_cb)

  registerOption<QCheckBox, bool>("log_ic_actions_cb", &Options::logIcActions,
                                  &Options::setLogIcActions);
  registerOption<QCheckBox, bool>("desync_logs_cb",
                                  &Options::desynchronisedLogsEnabled,
                                  &Options::setDesynchronisedLogsEnabled);
  registerOption<QCheckBox, bool>("log_text_cb", &Options::logToTextFileEnabled,
                                  &Options::setLogToTextFileEnabled);
  registerOption<QCheckBox, bool>("log_demo_cb", &Options::logToDemoFileEnabled,
                                  &Options::setLogToDemoFileEnabled);

  // DSGVO/Privacy tab

  FROM_UI(QTextBrowser, privacy_policy)
  ui_privacy_policy->setPlainText(tr("Getting privacy policy..."));

  // Char Download Manager tab

  FROM_UI(QTableWidget, download_table)
  FROM_UI(QTableWidget, server_download_table)
  registerOption<QTableWidget, TableData>("download_table", &Options::downloadManager,
                                  &Options::setDownloadManager);
  registerOption<QTableWidget, TableData>("server_download_table", &Options::serverDownloadManager,
                                  &Options::setServerDownloadManager);

  connect(ui_download_table, &QTableWidget::itemDoubleClicked,  this, [this](QTableWidgetItem* item) {
    int row = item->row();
    int column = item->column();

    if (column == 0) {
      QTableWidgetItem* linkItem = ui_download_table->item(row, column);
      if (linkItem) {
        QString downloadLink = linkItem->text();
        QUrl url(downloadLink);
        qDebug() << url.isValid();

        if (url.isValid()) {
          QDesktopServices::openUrl(url);
        } else {
          qWarning() << "Invalid URL: " << downloadLink;
        }
      }
    }
  });
  // I'll change this later...
  connect(ui_server_download_table, &QTableWidget::itemDoubleClicked,  this, [this](QTableWidgetItem* item) {
    int row = item->row();
    int column = item->column();

    if (column == 0) {
      QTableWidgetItem* linkItem = ui_server_download_table->item(row, column);
      if (linkItem) {
        QString downloadLink = linkItem->text();
        QUrl url(downloadLink);
        qDebug() << url.isValid();

        if (url.isValid()) {
          QDesktopServices::openUrl(url);
        } else {
          qWarning() << "Invalid URL: " << downloadLink;
        }
      }
    }
  });
  FROM_UI(QTabWidget, settings_tabs)
  updateValues();
}

void AOOptionsDialog::addMountItem(const QString &entry)
{
  auto *item = new QListWidgetItem();
  item->setData(Qt::UserRole, entry);

  MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
  if (manager != nullptr && manager->isPackageEntry(entry)) {
    const QString packageRoot = mountEntryPath(entry);
    const PackageMetadata metadata = readPackageMetadata(packageRoot);

    QString displayName = metadata.name;
    if (displayName.isEmpty()) {
      displayName = QFileInfo(packageRoot).fileName();
    }
    if (displayName.isEmpty()) {
      displayName = entry;
    }

    QStringList lines;
    lines.append(QStringLiteral("<b>%1</b>")
                     .arg(plainTextToHtml(displayName)));
    if (!metadata.description.isEmpty()) {
      lines.append(plainTextToHtml(metadata.description));
    }
    if (!metadata.author.isEmpty()) {
      lines.append(QStringLiteral("<i>%1</i>")
                       .arg(plainTextToHtml(tr("by %1").arg(metadata.author))));
    }
    item->setText(lines.join(QStringLiteral("<br>")));

    if (!metadata.icon.isEmpty()) {
      const QIcon icon(QDir(packageRoot).filePath(metadata.icon));
      if (!icon.isNull()) {
        item->setIcon(icon);
      }
    }
  }
  else {
    item->setText(plainTextToHtml(mountDisplayText(entry)));
  }

  ui_mount_list->addItem(item);
}

QString AOOptionsDialog::mountDisplayText(const QString &entry) const
{
  MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
  if (manager != nullptr && manager->isPackageEntry(entry)) {
    return tr("[Package] %1").arg(entry);
  }
  return tr("[External] %1").arg(entry);
}

QString AOOptionsDialog::mountEntryPath(const QString &entry) const
{
  if (entry.isEmpty()) {
    return {};
  }
  return QFileInfo(entry).isAbsolute()
             ? entry
             : QDir(get_app_dir_path()).filePath(entry);
}

bool AOOptionsDialog::mountEntryIsListed(const QString &entry) const
{
  for (int i = 1; i < ui_mount_list->count(); ++i) {
    if (ui_mount_list->item(i)->data(Qt::UserRole).toString() == entry) {
      return true;
    }
  }
  return false;
}

void AOOptionsDialog::populateMountList()
{
  if (ui_mount_list == nullptr) {
    return;
  }

  ui_mount_list->clear();
  auto *defaultMount =
      new QListWidgetItem(plainTextToHtml(tr("[Default] %1").arg("base")));
  // defaultMount->setFlags(Qt::ItemFlag::NoItemFlags);
  defaultMount->setData(Qt::UserRole, QString());
  ui_mount_list->addItem(defaultMount);

  MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
  if (manager == nullptr) {
    return;
  }

  for (const QString &entry : manager->entries()) {
    addMountItem(entry);
  }
  emit ui_mount_list->itemSelectionChanged();
}

void AOOptionsDialog::processPendingPackageRemovals()
{
  if (m_pendingPackageRemovals.isEmpty()) {
    return;
  }

  const QSet<QString> pending = m_pendingPackageRemovals;
  for (const QString &entry : pending) {
    const QString path = mountEntryPath(entry);
    if (path.isEmpty() || !QFileInfo::exists(path)) {
      m_pendingPackageRemovals.remove(entry);
      continue;
    }

    QString pathInTrash;
    if (QFile::moveToTrash(path, &pathInTrash)) {
      m_pendingPackageRemovals.remove(entry);
      asset_cache_dirty = true;
      continue;
    }

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(tr("Couldn't move package to Trash"));
    box.setText(
        tr("The package folder \"%1\" couldn't be moved to the system Trash.")
            .arg(entry));
    box.setInformativeText(
        tr("The Trash may not be available on this system. You can delete the "
           "folder permanently instead, or keep the package mounted."));

    auto *deleteButton =
        box.addButton(tr("Delete permanently"), QMessageBox::DestructiveRole);
    auto *keepButton =
        box.addButton(tr("Keep package"), QMessageBox::RejectRole);
    box.setDefaultButton(keepButton);
    box.setEscapeButton(keepButton);
    box.exec();

    bool removed = false;
    if (box.clickedButton() == deleteButton) {
      removed = QDir(path).removeRecursively();
      if (!removed) {
        QMessageBox::warning(
            this, tr("Couldn't delete package"),
            tr("The package folder \"%1\" couldn't be deleted. It will stay "
               "mounted.")
                .arg(entry));
      }
    }

    m_pendingPackageRemovals.remove(entry);
    if (removed) {
      asset_cache_dirty = true;
    }
    else if (!mountEntryIsListed(entry)) {
      addMountItem(entry);
    }
  }
}

void AOOptionsDialog::saveMountList()
{
  if (ui_mount_list == nullptr) {
    return;
  }

  QStringList entries;
  QSet<QString> listed;
  for (int i = 1; i < ui_mount_list->count(); ++i) {
    const QString entry =
        ui_mount_list->item(i)->data(Qt::UserRole).toString();
    if (entry.isEmpty()) {
      continue;
    }
    entries.append(entry);
    listed.insert(entry);
  }

  MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
  if (manager != nullptr) {
    const QStringList current = manager->entries();
    QSet<QString> currentSet;
    for (const QString &entry : current) {
      currentSet.insert(entry);
    }

    // Drop package entries whose folders disappeared while the dialog was
    // open, and merge packages that appeared after the list was populated.
    QStringList merged;
    for (const QString &entry : entries) {
      if (manager->isPackageEntry(entry) && !currentSet.contains(entry)) {
        continue;
      }
      merged.append(entry);
    }
    for (const QString &entry : current) {
      if (manager->isPackageEntry(entry) && !listed.contains(entry) &&
          !m_pendingPackageRemovals.contains(entry) &&
          QFileInfo(mountEntryPath(entry)).isDir()) {
        merged.append(entry);
      }
    }
    entries = merged;
  }

  m_applyingMountChange = true;
  if (manager != nullptr) {
    manager->setEntries(entries);
  }
  m_applyingMountChange = false;
  asset_cache_dirty = true;
}

void AOOptionsDialog::onMountsChanged()
{
  if (m_applyingMountChange || ui_mount_list == nullptr) {
    return;
  }

  MountManager *manager = ao_app != nullptr ? ao_app->mounts() : nullptr;
  if (manager == nullptr) {
    return;
  }

  const QStringList current = manager->entries();
  QSet<QString> currentPackages;
  for (const QString &entry : current) {
    if (manager->isPackageEntry(entry)) {
      currentPackages.insert(entry);
    }
  }

  // Remove packages whose folders were deleted.
  for (int i = ui_mount_list->count() - 1; i >= 1; --i) {
    const QString entry =
        ui_mount_list->item(i)->data(Qt::UserRole).toString();
    if (manager->isPackageEntry(entry) && !currentPackages.contains(entry)) {
      delete ui_mount_list->takeItem(i);
    }
  }

  // Append newly discovered packages, preserving the user's local order.
  QSet<QString> listed;
  for (int i = 1; i < ui_mount_list->count(); ++i) {
    listed.insert(ui_mount_list->item(i)->data(Qt::UserRole).toString());
  }
  for (const QString &entry : current) {
    if (!manager->isPackageEntry(entry) || listed.contains(entry) ||
        m_pendingPackageRemovals.contains(entry)) {
      continue;
    }
    addMountItem(entry);
    listed.insert(entry);
  }

  emit ui_mount_list->itemSelectionChanged();
}

void AOOptionsDialog::onTimestampFormatEdited()
{
  ui_log_timestamp_format_lbl->setText(
      tr("Log timestamp format:\n") +
      QDateTime::currentDateTime().toString(
          ui_log_timestamp_format_combobox->currentText()));
}

void AOOptionsDialog::timestampCbChanged(int state)
{
  ui_log_timestamp_format_combobox->setDisabled(state == 0);
}

#if (defined(_WIN32) || defined(_WIN64))
bool AOOptionsDialog::needsDefaultAudioDevice() { return true; }
#elif (defined(LINUX) || defined(__linux__))
bool AOOptionsDialog::needsDefaultAudioDevice() { return false; }
#elif defined __APPLE__
bool AOOptionsDialog::needsDefaultAudioDevice() { return true; }
#else
#error This operating system is not supported.
#endif
