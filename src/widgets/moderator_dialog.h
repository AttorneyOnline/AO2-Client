#pragma once

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

private:
    QWidget *ui_widget;
    QComboBox *ui_action_box;
    QComboBox *ui_selected_preset_box;
    QPushButton *ui_open_preset_editor_button;
    QSpinBox *ui_duration_box;
    QTextEdit *ui_details_edit;
    QDialogButtonBox *ui_button_box;
    int user_id;

private Q_SLOTS:
    void onAcceptedClicked();
    void onRejectedClicked();
    void onOpenPresetEditor();

signals:
};
