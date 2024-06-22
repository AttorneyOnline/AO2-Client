#include "moderator_dialog.h"
#include "gui_utils.h"
#include "options.h"

#include <QAbstractButton>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include <QPushButton>
#include <QSpinBox>
#include <QUiLoader>
#include <QVBoxLayout>

const QString ModeratorDialog::UI_FILE_PATH = "moderator_action_dialog.ui";

ModeratorDialog::ModeratorDialog(int f_id, QWidget *parent)
    : QWidget{parent}
    , user_id{f_id}
{
    QUiLoader l_loader(this);
    QFile l_uiFile(Options::getInstance().getUIAsset(UI_FILE_PATH));

    if (!l_uiFile.open(QFile::ReadOnly)) {
        qCritical() << "Unable to open file " << l_uiFile.fileName();
        return;
    }
    ui_widget = l_loader.load(&l_uiFile, this);
    auto l_layout = new QVBoxLayout(this);
    l_layout->addWidget(ui_widget);

    FROM_UI(QComboBox, action_box);
    FROM_UI(QComboBox, selected_preset_box);
    FROM_UI(QPushButton, open_preset_editor_button);
    connect(ui_open_preset_editor_button, &QPushButton::clicked, this, &ModeratorDialog::onOpenPresetEditor);
    FROM_UI(QSpinBox, duration_box);

    FROM_UI(QDialogButtonBox, button_box);
    connect(ui_button_box, &QDialogButtonBox::accepted, this, &ModeratorDialog::onAcceptedClicked);
    connect(ui_button_box, &QDialogButtonBox::rejected, this, &ModeratorDialog::onRejectedClicked);

    show();
}

ModeratorDialog::~ModeratorDialog()
{}

void ModeratorDialog::onAcceptedClicked()
{
    qDebug() << "Dialog confirmed.";
    close();
}

void ModeratorDialog::onRejectedClicked()
{
    qDebug() << "Dialog canceled";
    close();
}

void ModeratorDialog::onOpenPresetEditor()
{
    qDebug() << "Opening preset editor.";
}
