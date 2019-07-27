#include "aocharselect.h"
#include "aouiloader.h"

#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

AOCharSelect::AOCharSelect(QWidget *parent, AOApplication *p_ao_app)
  : QDialog(parent), ao_app(p_ao_app)
{
  QUiLoader loader(this);
  QFile uiFile(":/resource/ui/char_select.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QListWidget, char_list)
  FROM_UI(QDialogButtonBox, buttons)
}

int AOCharSelect::selectedCharacterIndex()
{
  return ui_char_list->selectedItems().isEmpty()
        ? -1
        : ui_char_list->selectedItems()[0]->data(Qt::UserRole).toInt();
}

void AOCharSelect::done(int retval)
{
  if (retval == QDialog::Accepted && ui_char_list->selectedItems().isEmpty())
  {
    // This shouldn't happen
    QMessageBox::warning(this, tr("Error"), tr("Please select a character."));
    return;
  }

  QDialog::done(retval);
}

void AOCharSelect::setCharacters(QVector<char_type> characters)
{
  ui_char_list->clear();
  int i = 0;
  for (const char_type &character : characters)
  {
    QString imagePath = ao_app->get_character_path(character.name,
                                                   "char_icon.png");
    auto item = new QListWidgetItem(QIcon(imagePath), QStringLiteral());
    item->setData(Qt::UserRole, i);

    if (character.taken)
      item->setFlags(item->flags() & ~Qt::ItemFlag::ItemIsEnabled);

    if (character.description.isEmpty())
    {
      item->setToolTip(character.name);
    }
    else
    {
      item->setToolTip(QStringLiteral("<h3>%1</h3>"
                                      "<p>%2</p>")
                       .arg(character.name)
                       .arg(character.description));
    }
    ui_char_list->addItem(item);
    i++;
  }
}

void AOCharSelect::on_charList_itemChanged(QListWidgetItem *item)
{
  ui_buttons->button(QDialogButtonBox::Ok)->setEnabled(item != nullptr);
}
