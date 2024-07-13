#include "moderator_dialog.h"

#include "aoapplication.h"
#include "gui_utils.h"
#include "options.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QUiLoader>
#include <QVBoxLayout>

const QString ModeratorDialog::UI_FILE_PATH = "moderator_action_dialog.ui";

ModeratorDialog::ModeratorDialog(int clientId, bool ban, AOApplication *ao_app, QWidget *parent)
    : QWidget{parent}
    , ao_app(ao_app)
    , m_client_id(clientId)
    , m_ban(ban)
{
  QFile file(Options::getInstance().getUIAsset(UI_FILE_PATH));
  if (!file.open(QFile::ReadOnly))
  {
    qFatal("Unable to open file %s", qPrintable(file.fileName()));
    return;
  }

  QUiLoader loader;
  ui_widget = loader.load(&file, this);
  auto layout = new QVBoxLayout(this);
  layout->addWidget(ui_widget);

  FROM_UI(QComboBox, action);
  FROM_UI(QSpinBox, duration);
  FROM_UI(QLabel, duration_label);
  FROM_UI(QCheckBox, permanent);
  FROM_UI(QTextEdit, details);
  FROM_UI(QDialogButtonBox, button_box);

  if (m_ban)
  {
    ui_action->addItem(tr("Ban"));
  }
  else
  {
    ui_action->addItem(tr("Kick"));
  }

  ui_duration->setVisible(m_ban);
  ui_duration_label->setVisible(m_ban);
  ui_permanent->setVisible(m_ban);

  connect(ui_button_box, &QDialogButtonBox::accepted, this, &ModeratorDialog::onAcceptedClicked);
  connect(ui_button_box, &QDialogButtonBox::rejected, this, &ModeratorDialog::close);
}

ModeratorDialog::~ModeratorDialog()
{}

void ModeratorDialog::onAcceptedClicked()
{
  QString reason = ui_details->toPlainText();
  if (reason.isEmpty())
  {
    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to confirm without a reason?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
  }

  bool permanent = ui_permanent->isChecked();
  if (permanent)
  {
    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to ban permanently?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
  }

  QStringList arglist;
  arglist.append(QString::number(m_client_id));
  if (m_ban)
  {
    if (permanent)
    {
      arglist.append("-1");
    }
    else
    {
      arglist.append(QString::number(ui_duration->value()));
    }
  }
  else
  {
    arglist.append("0");
  }
  arglist.append(reason);

  ao_app->send_server_packet(AOPacket("MA", arglist));

  close();
}
