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
#include <QtWidgets/QWidget>

#include <QDirIterator>
#include <QListWidget>
#include <QTextBrowser>
#include <QTextStream>

#define FROM_UI(type, name); ui_##name = findChild<type *>(#name);

class Lobby;
class Courtroom;
class AOApplication;
class Options;

struct OptionEntry {
  std::function<void()> load;
  std::function<void()> save;
};

class AOOptionsDialog : public QWidget {
  Q_OBJECT
public:
  explicit AOOptionsDialog(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);

private:
  AOApplication *ao_app;
  Options options;

  QWidget* l_settings_widget;

  QVBoxLayout *ui_vertical_layout;
  QTabWidget *ui_settings_tabs;

  QWidget *ui_gameplay_tab;
  QWidget *ui_form_layout_widget;
  QFormLayout *ui_gameplay_form;

  QLabel *ui_theme_label;
  QComboBox *ui_theme_combobox;
  QLabel *ui_subtheme_label;
  QComboBox *ui_subtheme_combobox;
  QPushButton *ui_theme_reload_button;
  QPushButton *ui_theme_folder_button;
  QLabel *ui_evidence_double_click_lbl;
  QCheckBox *ui_evidence_double_click_cb;
  QLabel *ui_animated_theme_lbl;
  QCheckBox *ui_animated_theme_cb;
  QFrame *ui_theme_log_divider;
  QLabel *ui_stay_time_lbl;
  QSpinBox *ui_stay_time_spinbox;
  QLabel *ui_instant_objection_lbl;
  QCheckBox *ui_instant_objection_cb;
  QLabel *ui_text_crawl_lbl;
  QSpinBox *ui_text_crawl_spinbox;
  QLabel *ui_chat_ratelimit_lbl;
  QSpinBox *ui_chat_ratelimit_spinbox;
  QFrame *ui_log_names_divider;
  QLineEdit *ui_username_textbox;
  QLabel *ui_username_lbl;
  QLabel *ui_showname_lbl;
  QCheckBox *ui_showname_cb;
  QLabel *ui_default_showname_lbl;
  QLineEdit *ui_default_showname_textbox;
  QFrame *ui_net_divider;
  QLabel *ui_ms_lbl;
  QLineEdit *ui_ms_textbox;
  QLabel *ui_discord_lbl;
  QCheckBox *ui_discord_cb;
  QLabel *ui_language_label;
  QComboBox *ui_language_combobox;
  QLabel *ui_scaling_label;
  QComboBox *ui_scaling_combobox;

  QLabel *ui_shake_lbl;
  QCheckBox *ui_shake_cb;
  QLabel *ui_effects_lbl;
  QCheckBox *ui_effects_cb;
  QLabel *ui_framenetwork_lbl;
  QCheckBox *ui_framenetwork_cb;

  QLabel *ui_colorlog_lbl;
  QCheckBox *ui_colorlog_cb;

  QLabel *ui_stickysounds_lbl;
  QCheckBox *ui_stickysounds_cb;

  QLabel *ui_stickyeffects_lbl;
  QCheckBox *ui_stickyeffects_cb;

  QLabel *ui_stickypres_lbl;
  QCheckBox *ui_stickypres_cb;

  QLabel *ui_customchat_lbl;
  QCheckBox *ui_customchat_cb;

  QLabel *ui_sticker_lbl;
  QCheckBox *ui_sticker_cb;

  QLabel *ui_continuous_lbl;
  QCheckBox *ui_continuous_cb;

  QLabel *ui_category_stop_lbl;
  QCheckBox *ui_category_stop_cb;

  QLabel *ui_sfx_on_idle_lbl;
  QCheckBox *ui_sfx_on_idle_cb;

  QWidget *ui_callwords_tab;
  QWidget *ui_callwords_widget;
  QVBoxLayout *ui_callwords_layout;
  QPlainTextEdit *ui_callwords_textbox;
  QLabel *ui_callwords_explain_lbl;
  QCheckBox *ui_callwords_char_textbox;

  QWidget *ui_audio_tab;
  QWidget *ui_audio_widget;
  QFormLayout *ui_audio_layout;
  QLabel *ui_audio_device_lbl;
  QComboBox *ui_audio_device_combobox;
  QFrame *ui_audio_volume_divider;
  QSpinBox *ui_music_volume_spinbox;
  QLabel *ui_music_volume_lbl;
  QSpinBox *ui_sfx_volume_spinbox;
  QSpinBox *ui_blips_volume_spinbox;
  QSpinBox *ui_suppress_audio_spinbox;
  QLabel *ui_sfx_volume_lbl;
  QLabel *ui_blips_volume_lbl;
  QLabel *ui_suppress_audio_lbl;
  QFrame *ui_volume_blip_divider;
  QSpinBox *ui_bliprate_spinbox;
  QLabel *ui_bliprate_lbl;
  QCheckBox *ui_blank_blips_cb;
  QLabel *ui_blank_blips_lbl;
  QLabel *ui_loopsfx_lbl;
  QCheckBox *ui_loopsfx_cb;
  QLabel *ui_objectmusic_lbl;
  QCheckBox *ui_objectmusic_cb;
  QLabel *ui_disablestreams_lbl;
  QCheckBox *ui_disablestreams_cb;
  QDialogButtonBox *ui_settings_buttons;

  QWidget *ui_casing_tab;
  QWidget *ui_casing_widget;
  QFormLayout *ui_casing_layout;
  QLabel *ui_casing_supported_lbl;
  QLabel *ui_casing_enabled_lbl;
  QCheckBox *ui_casing_enabled_cb;
  QLabel *ui_casing_def_lbl;
  QCheckBox *ui_casing_def_cb;
  QLabel *ui_casing_pro_lbl;
  QCheckBox *ui_casing_pro_cb;
  QLabel *ui_casing_jud_lbl;
  QCheckBox *ui_casing_jud_cb;
  QLabel *ui_casing_jur_lbl;
  QCheckBox *ui_casing_jur_cb;
  QLabel *ui_casing_steno_lbl;
  QCheckBox *ui_casing_steno_cb;
  QLabel *ui_casing_cm_lbl;
  QCheckBox *ui_casing_cm_cb;
  QLabel *ui_casing_cm_cases_lbl;
  QLineEdit *ui_casing_cm_cases_textbox;

  QWidget *ui_assets_tab;
  QVBoxLayout *ui_assets_tab_layout;
  QGridLayout *ui_mount_buttons_layout;
  QLabel *ui_asset_lbl;
  QListWidget *ui_mount_list;
  QPushButton *ui_mount_add;
  QPushButton *ui_mount_remove;
  QPushButton *ui_mount_up;
  QPushButton *ui_mount_down;
  QPushButton *ui_mount_clear_cache;

  QWidget *ui_logging_tab;
  QWidget *ui_form_logging_widget;
  QFormLayout *ui_logging_form;

  /**
   * Option for log direction. Supported options are downwards and wrong way.
   */
  QLabel *ui_downwards_lbl;
  QCheckBox *ui_downwards_cb;

  /**
   * Option for log length. Controls how many IC-log entries are kept before it
   * autowraps.
   */
  QLabel *ui_length_lbl;
  QSpinBox *ui_length_spinbox;

  /**
   * Option for log newline. Controls if the IC-log contains newlines or as one
   * consecutive string.
   */
  QLabel *ui_log_newline_lbl;
  QCheckBox *ui_log_newline_cb;

  /**
   * Option for log margin. Controls how many pixels are between each log entry.
   */
  QLabel *ui_log_margin_lbl;
  QSpinBox *ui_log_margin_spinbox;

  /**
   * Option for timestamp format. A checkmark to enable the timestamp dropdown.
   * Why does this exist? Are we Dorico now?
   */
  QLabel *ui_log_timestamp_lbl;
  QCheckBox *ui_log_timestamp_cb;

  /**
   * Option for timestamp format. Dropdown to select the preferred format.
   */
  QLabel *ui_log_timestamp_format_lbl;
  QComboBox *ui_log_timestamp_format_combobox;

  /**
   * Option for desynched IC-log and viewport. Controls if entires are appended
   * to the IC-log before displayed in the viewport.
   */
  QLabel *ui_desync_logs_lbl;
  QCheckBox *ui_desync_logs_cb;

  /**
   * Option for logging IC-actions. Will add shouts, evidence or music changes
   * to the IC-log.
   */
  QLabel *ui_log_ic_actions_lbl;
  QCheckBox *ui_log_ic_actions_cb;

  /**
   * Option to enable logging. If enabled client will save all messages to the
   * log folder.
   */
  QLabel *ui_log_text_lbl;
  QCheckBox *ui_log_text_cb;

  /**
   * Option to enable demo logging. If enabled, client will save a demo file for
   * replay trough the demo server.
   */
  QLabel *ui_log_demo_lbl;
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

  bool needs_default_audiodev();
  void update_values();

  QVector<OptionEntry> optionEntries;

  template <typename T, typename V>
  void setWidgetData(T *widget, const V &value);

  template <typename T, typename V> V widgetData(T *widget) const;

  template <typename T, typename V>
  void registerOption(const QString &widgetName, V (Options::*getter)() const,
                      void (Options::*setter)(V));

signals:

public slots:
  void save_pressed();
  void discard_pressed();
  void button_clicked(QAbstractButton *button);
  void on_timestamp_format_edited();
  void timestamp_cb_changed(int state);
  void on_reload_theme_clicked();
  void theme_changed(int i);
};

#endif // AOOPTIONSDIALOG_H
