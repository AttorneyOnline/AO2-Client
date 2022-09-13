#ifndef AOOPTIONSDIALOG_H
#define AOOPTIONSDIALOG_H

#include "bass.h"
#include "options.h"

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QWidget>

#include <QDirIterator>
#include <QListWidget>
#include <QTextBrowser>
#include <QTextStream>

class Lobby;
class Courtroom;
class AOApplication;
class Options;

struct OptionEntry {
  std::function<void()> load;
  std::function<void()> save;
};

class AOOptionsDialog : public QDialog {
  Q_OBJECT
public:
  explicit AOOptionsDialog(QDialog *parent = nullptr, AOApplication *p_ao_app = nullptr);
private:
  AOApplication *ao_app;

  //Dialog interaction buttons. Save/Discard/Restore Defaults
  QDialogButtonBox *ui_settings_buttons;

  // The gameplay tab
  QWidget* ui_settings_widget;
  QComboBox *ui_theme_combobox;
  QComboBox *ui_subtheme_combobox;
  QPushButton *ui_theme_reload_button;
  QPushButton *ui_theme_folder_button;
  QCheckBox *ui_evidence_double_click_cb;
  QCheckBox *ui_animated_theme_cb;
  QSpinBox *ui_stay_time_spinbox;
  QCheckBox *ui_instant_objection_cb;
  QSpinBox *ui_text_crawl_spinbox;
  QSpinBox *ui_chat_ratelimit_spinbox;
  QFrame *ui_log_names_divider;
  QLineEdit *ui_username_textbox;
  QCheckBox *ui_showname_cb;
  QLineEdit *ui_default_showname_textbox;
  QFrame *ui_net_divider;
  QLineEdit *ui_ms_textbox;
  QCheckBox *ui_discord_cb;
  QLabel *ui_language_label;
  QComboBox *ui_language_combobox;
  QLabel *ui_scaling_label;
  QComboBox *ui_scaling_combobox;
  QCheckBox *ui_shake_cb;
  QCheckBox *ui_effects_cb;
  QCheckBox *ui_framenetwork_cb;
  QCheckBox *ui_colorlog_cb;
  QCheckBox *ui_stickysounds_cb;
  QCheckBox *ui_stickyeffects_cb;
  QCheckBox *ui_stickypres_cb;
  QCheckBox *ui_customchat_cb;
  QCheckBox *ui_sticker_cb;
  QCheckBox *ui_continuous_cb;
  QCheckBox *ui_category_stop_cb;
  QCheckBox *ui_sfx_on_idle_cb;

  //The callwords tab
  QPlainTextEdit *ui_callwords_textbox;
  QCheckBox *ui_callwords_char_textbox;

  //The audio tab
  QWidget *ui_audio_tab;
  QWidget *ui_audio_widget;
  QFormLayout *ui_audio_layout;
  QComboBox *ui_audio_device_combobox;
  QSpinBox *ui_music_volume_spinbox;
  QSpinBox *ui_sfx_volume_spinbox;
  QSpinBox *ui_blips_volume_spinbox;
  QSpinBox *ui_suppress_audio_spinbox;
  QFrame *ui_volume_blip_divider;
  QSpinBox *ui_bliprate_spinbox;
  QCheckBox *ui_blank_blips_cb;
  QCheckBox *ui_loopsfx_cb;
  QCheckBox *ui_objectmusic_cb;
  QCheckBox *ui_disablestreams_cb;

  //The casing tab
  QLabel *ui_casing_supported_lbl;
  QGroupBox *ui_casing_enabled_box;
  QCheckBox *ui_casing_def_cb;
  QCheckBox *ui_casing_pro_cb;
  QCheckBox *ui_casing_jud_cb;
  QCheckBox *ui_casing_jur_cb;
  QCheckBox *ui_casing_steno_cb;
  QCheckBox *ui_casing_cm_cb;
  QLineEdit *ui_casing_cm_cases_textbox;

  //The asset tab
  QListWidget *ui_mount_list;
  QPushButton *ui_mount_add;
  QPushButton *ui_mount_remove;
  QPushButton *ui_mount_up;
  QPushButton *ui_mount_down;
  QPushButton *ui_mount_clear_cache;

  //The logging tab
  QCheckBox *ui_downwards_cb;
  QSpinBox *ui_length_spinbox;
  QCheckBox *ui_log_newline_cb;
  QSpinBox *ui_log_margin_spinbox;
  QLabel *ui_log_timestamp_format_lbl;
  QCheckBox *ui_log_timestamp_cb;
  QComboBox *ui_log_timestamp_format_combobox;
  QCheckBox *ui_desync_logs_cb;
  QCheckBox *ui_log_ic_actions_cb;
  QCheckBox *ui_log_text_cb;
  QCheckBox *ui_log_demo_cb;

  /**
   * Allows the AO2 master server hoster to go broke.
   */
  QWidget *ui_privacy_tab;
  QVBoxLayout *ui_privacy_layout;
  QCheckBox *ui_privacy_optout_cb;
  QFrame *ui_privacy_separator;
  QTextBrowser *ui_privacy_policy;

  bool asset_cache_dirty = false;

  bool needsDefaultAudioDevice();
  void populateAudioDevices();
  void updateValues();

  QVector<OptionEntry> optionEntries;

  template <typename T, typename V>
  void setWidgetData(T *widget, const V &value);

  template <typename T, typename V> V widgetData(T *widget) const;

  template <typename T, typename V>
  void registerOption(const QString &widgetName, V (Options::*getter)() const,
                      void (Options::*setter)(V));

signals:

public slots:
  void savePressed();
  void discardPressed();
  void buttonClicked(QAbstractButton *button);
  void onTimestampFormatEdited();
  void timestampCbChanged(int state);
  void onReloadThemeClicked();
  void themeChanged(int i);
};

#endif // AOOPTIONSDIALOG_H
