#include "widgets/aoevidence.h"
#include "aouiloader.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

AOEvidence::AOEvidence(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/evidence.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QPlainTextEdit, description)
  FROM_UI(QLabel, image)
  FROM_UI(QTextEdit, name)
  FROM_UI(QListWidget, list)
  FROM_UI(QPushButton, present_button)
  FROM_UI(QPushButton, delete_button)
  FROM_UI(QStackedWidget, pages)

  // There are some style decisions to be made for the new evidence button,
  // and therefore its creation cannot be deferred to this constructor code.
  // It must be made ahead of time in the UI file.
  if (ui_list->count() == 0)
  {
    QMessageBox::critical(this, tr("UI Design Error"),
                          tr("The evidence UI file must contain a \"new evidence\" entry"
                             "in the evidence list.\n"
                             "The program cannot continue. Press OK to crash the program"
                             "gracefully."));
    throw std::exception();
  }

  // Set "New Evidence" entry (the last entry) to -1
  ui_list->item(ui_list->count() - 1)->setData(Qt::UserRole, -1);
}

bool AOEvidence::presenting()
{
  return ui_present_button->isChecked();
}

int AOEvidence::selectedEvidenceId()
{
  if (ui_list->currentItem() == nullptr)
    return -1;

  return ui_list->currentItem()->data(Qt::UserRole).toInt();
}

void AOEvidence::showEvidence(int id)
{
  if (id > -1)
  {
    ui_pages->setCurrentIndex(info_page_id);
    evi_type &evi = evidence[selectedEvidenceId()];

    if (debounceCause != ui_name)
      ui_name->document()->setPlainText(evi.name);
    if (debounceCause != ui_description)
      ui_description->document()->setPlainText(evi.description);
    if (debounceCause != ui_image)
      ui_image->setPixmap(QPixmap(ao_app->get_evidence_path(evi.image)));
  }
  else
  {
    ui_pages->setCurrentIndex(empty_page_id);
  }
}

void AOEvidence::addMessageData(chat_message_type &message)
{
  // the evidence index is shifted by 1 because 0 is no evidence, per legacy standards
  message.evidence = selectedEvidenceId() + 1;
}

void AOEvidence::setupDebouncer()
{
  debounceTimer = new QTimer(this);
  debounceTimer->setSingleShot(true);
  debounceTimer->setInterval(debounceInterval);
  connect(debounceTimer, &QTimer::timeout, this, &AOEvidence::debounceFired);
  connect(debounceTimer, &QTimer::timeout, debounceTimer, &QTimer::deleteLater);
}

void AOEvidence::debounceFired()
{
  debounceTimer->deleteLater();

  int id = selectedEvidenceId();
  emit evidenceEdited(id, evidence[id]);
}

void AOEvidence::on_list_itemChanged(QListWidgetItem *item)
{
  // Force send changes immediately if debounce timer is running
  if (debounceTimer)
    debounceFired();

  // If item is now null, switch to the empty page
  // Otherwise, switch to the evidence info page and populate entries
  if (item != nullptr)
    showEvidence(selectedEvidenceId());
  else
    ui_pages->setCurrentIndex(empty_page_id);
}

void AOEvidence::on_list_itemActivated(QListWidgetItem *item)
{
  // Check if the item that was double-clicked was the "new evidence" button.
  // If it wasn't, then we don't care
  if (item != nullptr && selectedEvidenceId() == -1)
  {
    evi_type newEvidence;
    newEvidence.name = "New Evidence";
    newEvidence.description = "";
    newEvidence.image = "";

    emit evidenceAdded(newEvidence);
  }
}

void AOEvidence::on_name_textChanged()
{
  if (!debounceTimer)
    setupDebouncer();
  debounceCause = ui_name;

  evidence[selectedEvidenceId()].name = ui_name->document()->toRawText();
}

void AOEvidence::on_description_textChanged()
{
  if (!debounceTimer)
    setupDebouncer();
  debounceCause = ui_description;

  evidence[selectedEvidenceId()].description = ui_description->document()->toRawText();
}

void AOEvidence::on_image_linkActivated()
{
  // This is my least favorite feature
  QDir evidenceDir(ao_app->get_evidence_path(""));
  QString filename = QFileDialog::getOpenFileName(this, "Select evidence image",
                                                  evidenceDir.absolutePath(),
                                                  tr("Images (*.png *.gif)"));

  if (filename.isEmpty())
    return;

  QString relativePath = evidenceDir.relativeFilePath(filename);
  if (relativePath.startsWith("..")) {
    QMessageBox::critical(this, tr("Error adding evidence"),
                          tr("You can only use evidence images that are in the"
                          "evidence folder."));
    return;
  }

  ui_image->setPixmap(QPixmap(evidenceDir.absoluteFilePath(relativePath)));

  if (debounceTimer)
    debounceFired();
  debounceCause = ui_image;

  evidence[selectedEvidenceId()].image = relativePath;
}

void AOEvidence::on_deleteButton_clicked()
{
  debounceTimer->stop();
  debounceTimer->deleteLater();

  emit evidenceDeleted(selectedEvidenceId());
}

void AOEvidence::setEvidenceList(QVector<evi_type> list)
{
  evidence = list;
  int originalIndex = ui_list->currentRow();

  // Update existing items
  for (int i = 0; i < ui_list->count() - 1 && i < list.count(); i++)
  {
    QString imagePath = ao_app->get_evidence_path(evidence[i].image);
    auto item = ui_list->item(i);
    item->setIcon(QIcon(imagePath));
    item->setText(evidence[i].name);
  }

  // Remove excess items
  while (list.length() < ui_list->count() - 1)
    ui_list->takeItem(list.length());

  // Add new items
  for (int i = ui_list->count() - 1; i < list.length(); i++)
  {
    QString imagePath = ao_app->get_evidence_path(evidence[i].image);
    auto item = new QListWidgetItem(QIcon(imagePath), evidence[i].name);
    item->setData(Qt::UserRole, i);
    ui_list->insertItem(i, item);
  }

  // Clear the selection if it no longer exists
  if (originalIndex >= ui_list->count() - 1)
    ui_list->clearSelection();
  else
    showEvidence(selectedEvidenceId());

  debounceCause.clear();
}

void AOEvidence::togglePresenting(bool presenting)
{
  ui_present_button->setChecked(presenting);
}
