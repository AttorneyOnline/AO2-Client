#include "moderator_dialog.h"
#include "datatypes.h"
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
    FROM_UI(QSpinBox, duration_box);
    FROM_UI(QTextEdit, details_edit);

    FROM_UI(QDialogButtonBox, button_box);
    connect(ui_button_box, &QDialogButtonBox::accepted, this, &ModeratorDialog::onAcceptedClicked);
    connect(ui_button_box, &QDialogButtonBox::rejected, this, &ModeratorDialog::onRejectedClicked);

    QStringList commands = loader.commands();
    ui_action_box->addItems(commands);

    connect(ui_action_box, &QComboBox::currentTextChanged, this, [this](QString text){
       ui_selected_preset_box->clear();
       for (const PresetData &preset : loader.commandPresets(text))
       {
         ui_selected_preset_box->addItem(preset.label);
       }
    });

    connect(ui_selected_preset_box, &QComboBox::currentIndexChanged, this, [this](int index) {
      PresetData preset = loader.commandPresets(ui_action_box->currentText()).at(index);
      ui_details_edit->setText(preset.details);
    });

    show();
}

ModeratorDialog::~ModeratorDialog()
{}

ModerationPresetLoader *ModeratorDialog::presetLoader()
{
  return &loader;
}

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
