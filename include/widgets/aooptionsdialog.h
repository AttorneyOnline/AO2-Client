#ifndef AOOPTIONSDIALOG_H
#define AOOPTIONSDIALOG_H

#include "aoapplication.h"
#include "contrib/bass.h"

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

#include <QDirIterator>
#include <QTextStream>

struct OptionEntry
{
  std::function<void()> load;
  std::function<void()> save;
};

class AOOptionsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit AOOptionsDialog(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);

private:
    AOApplication *ao_app;
    Options options;

    QVector<OptionEntry> optionEntries;

    QComboBox *ui_theme;
    QComboBox *ui_audio_device;

    QLineEdit *ui_viewport_scale_width;
    QLineEdit *ui_viewport_scale_height;
    QLabel *ui_viewport_ratio;

    QDialogButtonBox *ui_dialog_buttons;

    template<typename T, typename V>
    void setWidgetData(T *widget, const V &value);

    template<typename T, typename V>
    V widgetData(T *widget) const;

    template<typename T, typename V>
    void registerOption(const QString &widgetName, V (Options::*getter)() const,
                        void (Options::*setter)(V));
    bool needsDefaultAudioDevice();

signals:

private slots:
  void populateThemes();
  void populateAudioDevices();
  void calculateViewportRatio();

  void loadAll();
  void saveAll();

  void openLogFolder();
};

#endif // AOOPTIONSDIALOG_H
