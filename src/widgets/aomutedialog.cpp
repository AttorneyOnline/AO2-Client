#include "widgets/aomutedialog.h"

#include <QAbstractButton>
#include <QUiLoader>
#include <QVBoxLayout>

AOMuteDialog::AOMuteDialog(QWidget *parent)
  : QDialog(parent)
{
  QUiLoader loader(this);
  QFile uiFile(":/resource/ui/mute.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_mute_list = findChild<QListWidget *>("mute_list");
  ui_buttons = findChild<QDialogButtonBox *>("buttons");
}

void AOMuteDialog::setCharacters(QVector<QString> characters)
{
  ui_mute_list->clear();

  int cid = 0;
  for (const QString &character : characters)
  {
    auto item = new QListWidgetItem(character);
    item->setData(Qt::UserRole, cid++);
    ui_mute_list->addItem(item);
  }
}

void AOMuteDialog::on_muteList_itemChanged(QListWidgetItem *item)
{
  // POTENTIAL BUG: itemChanged could trigger for changes that were not
  // check states.
  emit mute(item->data(Qt::UserRole).toInt(),
            item->checkState() == Qt::CheckState::Checked);
}

void AOMuteDialog::on_buttons_clicked(QAbstractButton *button)
{
  if (ui_buttons->standardButton(button) == QDialogButtonBox::Reset) {
    ui_mute_list->reset();
  }
}
