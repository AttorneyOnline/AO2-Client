#ifndef AOOPTIONSDIALOG_H
#define AOOPTIONSDIALOG_H

#include "aoapplication.h"
#include "bass.h"

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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class AOOptionsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit AOOptionsDialog(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);

private:
    AOApplication *ao_app;

    QVBoxLayout *verticalLayout;
    QTabWidget *SettingsTabs;
    QWidget *GameplayTab;
    QWidget *formLayoutWidget;
    QFormLayout *GameplayForm;
    QLabel *ThemeLabel;
    QComboBox *ThemeCombobox;
    QFrame *ThemeLogDivider;
    QLabel *DownwardsLabel;
    QCheckBox *DownwardCheckbox;
    QLabel *LengthLabel;
    QSpinBox *LengthSpinbox;
    QFrame *LogNamesDivider;
    QLineEdit *UsernameLineEdit;
    QLabel *UsernameLabel;
    QLabel *ShownameLabel;
    QCheckBox *ShownameCheckbox;
    QFrame *NetDivider;
    QLabel *MasterServerLabel;
    QLineEdit *MasterServerLineEdit;
    QLabel *DiscordLabel;
    QCheckBox *DiscordCheckBox;
    QWidget *CallwordsTab;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *CallwordsLayout;
    QPlainTextEdit *CallwordsTextEdit;
    QLabel *CallwordsExplainLabel;
    QCheckBox *CharacterCallwordsCheckbox;
    QWidget *AudioTab;
    QWidget *formLayoutWidget_2;
    QFormLayout *AudioForm;
    QLabel *AudioDevideLabel;
    QComboBox *AudioDeviceCombobox;
    QFrame *DeviceVolumeDivider;
    QSpinBox *MusicVolumeSpinbox;
    QLabel *MusicVolumeLabel;
    QSpinBox *SFXVolumeSpinbox;
    QSpinBox *BlipsVolumeSpinbox;
    QLabel *SFXVolumeLabel;
    QLabel *BlipsVolumeLabel;
    QFrame *VolumeBlipDivider;
    QSpinBox *BlipRateSpinbox;
    QLabel *BlipRateLabel;
    QCheckBox *BlankBlipsCheckbox;
    QLabel *BlankBlipsLabel;
    QDialogButtonBox *SettingsButtons;

signals:

public slots:
    void save_pressed();
    void discard_pressed();
};

#endif // AOOPTIONSDIALOG_H
