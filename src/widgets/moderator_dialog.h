#pragma once

#include "moderationpresetloader.h"

#include <QWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>

class ModeratorDialog : public QWidget
{
    Q_OBJECT
public:
    static const QString UI_FILE_PATH;

    explicit ModeratorDialog(int f_id, QWidget *parent = nullptr);
    virtual ~ModeratorDialog();

    ModerationPresetLoader *presetLoader();

  private:
    QWidget *ui_widget;
    QComboBox *ui_action_box;
    QComboBox *ui_selected_preset_box;
    QSpinBox *ui_duration_box;
    QTextEdit *ui_details_edit;
    QDialogButtonBox *ui_button_box;
    int user_id;

    ModerationPresetLoader loader;

  private Q_SLOTS:
    void onAcceptedClicked();
    void onRejectedClicked();
    void onOpenPresetEditor();

signals:
};
