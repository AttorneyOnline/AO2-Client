#include "widgets/AOPairDialog.h"

#include <QAbstractButton>
#include <QUiLoader>
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QFile>

#include "datatypes.h"

AOPairDialog::AOPairDialog(QWidget *parent)
  : QDialog(parent)
{
  QUiLoader loader(this);
  QFile uiFile(":/resource/ui/pair.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  setWindowTitle(tr("Pair"));

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_char_list = findChild<QListWidget *>("char_list");
  ui_offset_slider = findChild<QSlider *>("offset_slider");
  ui_buttons = findChild<QDialogButtonBox *>("buttons");
}

void AOPairDialog::setCharacters(const QVector<char_type> &characters)
{
  ui_char_list->clear();

  for (const auto &character : characters)
  {
    auto item = new QListWidgetItem(character.name);
    ui_char_list->addItem(item);
  }
}

void AOPairDialog::setCurrentPair(int cid) {
  if (cid == -1)
    ui_char_list->clearSelection();
  else
    ui_char_list->setCurrentRow(cid);
}

void AOPairDialog::setPairOffset(int offset) {
  ui_offset_slider->setValue(offset);
}

void AOPairDialog::on_buttons_clicked(QAbstractButton *button)
{
  switch (ui_buttons->standardButton(button)) {
  case QDialogButtonBox::Reset:
    ui_char_list->clearSelection();
    ui_offset_slider->setValue(0);
    break;
  case QDialogButtonBox::Ok:
    emit pairChanged(ui_char_list->selectedItems().count()
      ? ui_char_list->currentRow()
      : -1);
    emit offsetChanged(ui_offset_slider->value());
    /* fallthrough */
  case QDialogButtonBox::Cancel:
    close();
    break;
  }
}
