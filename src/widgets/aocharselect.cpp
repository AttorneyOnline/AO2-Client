#include "aocharselect.h"
#include "aouiloader.h"
#include "file_functions.h"

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
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  setWindowTitle(tr("Select Character"));
  resize(520, 400);

  FROM_UI(QListWidget, char_list)
  FROM_UI(QDialogButtonBox, buttons)
  FROM_UI(QPushButton, spectate)

  connect(ui_buttons, &QDialogButtonBox::accepted,
          this, &QDialog::accept);
  connect(ui_buttons, &QDialogButtonBox::rejected,
          this, &QDialog::reject);
  connect(ui_spectate, &QPushButton::clicked,
          this, [&] {
    ui_char_list->clearSelection();
    accept();
  });
  connect(ui_char_list, &QListWidget::itemActivated,
          this, &QDialog::accept);
}

int AOCharSelect::selectedCharacterIndex()
{
  return ui_char_list->selectedItems().isEmpty()
        ? -1
        : ui_char_list->selectedItems()[0]->data(Qt::UserRole).toInt();
}

void AOCharSelect::done(int retval)
{
  if (retval == QDialog::Accepted && sender() != ui_spectate
      && ui_char_list->selectedItems().isEmpty())
  {
    QMessageBox::warning(this, tr("Error"), tr("Please select a character."));
    return;
  }

  if (retval == QDialog::Accepted)
  {
    // Submit and await response from server
    emit characterSelected(selectedCharacterIndex());
    return;
  }

  QDialog::done(retval);
}

void AOCharSelect::responseReceived()
{
  // For now, since the network protocol only sends successes instead of
  // failures, we can keep this slot without any parameters indicating
  // failure.
  QDialog::done(QDialog::Accepted);
  deleteLater();
}

void AOCharSelect::setCharacters(QVector<char_type> characters)
{
  int oldSelected = ui_char_list->currentRow();

  ui_char_list->clear();

  int i = 0;
  for (const char_type &character : characters)
  {
    QString imagePath = ao_app->get_character_path(character.name,
                                                   "char_icon.png");
    if (!file_exists(imagePath))
      imagePath = ao_app->get_default_theme_path("char_selector.png");

    // nullptr is required to select an overload that doesn't require
    // specifying any text. If any text at all is specified, the
    // Qt::DisplayRole role is touched, which adds an annoying space
    // at the bottom of the item.
    auto item = new QListWidgetItem(QIcon(imagePath), nullptr);
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

  if (oldSelected >= 0 && oldSelected < characters.size()
      && !characters[oldSelected].taken)
  {
    ui_char_list->setCurrentRow(i);
  }
}

void AOCharSelect::on_char_list_itemChanged(QListWidgetItem *item)
{
  ui_buttons->button(QDialogButtonBox::Ok)->setEnabled(item != nullptr);
}
