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

  setWindowTitle(tr("Evidence"));

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QPlainTextEdit, description)
  FROM_UI(QPushButton, image)
  FROM_UI(QTextEdit, name)
  FROM_UI(QListWidget, list)
  FROM_UI(QPushButton, present_button)
  FROM_UI(QPushButton, delete_button)
  FROM_UI(QStackedWidget, pages)
  FROM_UI(QWidget, evidence_info_page)
  FROM_UI(QWidget, empty_page)

  connect(ui_list, &QListWidget::currentItemChanged,
          this, &AOEvidence::on_list_currentItemChanged);
  connect(ui_list, &QListWidget::itemActivated,
          this, &AOEvidence::on_list_itemActivated);
  connect(ui_name, &QTextEdit::textChanged,
          this, &AOEvidence::on_name_textChanged);
  connect(ui_description, &QPlainTextEdit::textChanged,
          this, &AOEvidence::on_description_textChanged);
  connect(ui_image, &QPushButton::clicked,
          this, &AOEvidence::on_image_clicked);
  connect(ui_delete_button, &QPushButton::clicked,
          this, &AOEvidence::on_delete_button_clicked);

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

evi_type *AOEvidence::selectedEvidence()
{
  auto id = selectedEvidenceId();
  if (id == -1 || id >= evidence.count())
    return nullptr;
  return &evidence[id];
}

void AOEvidence::showEvidence(const evi_type *evi)
{
  if (evi)
  {
    ui_pages->setCurrentWidget(ui_evidence_info_page);

    // Prevent textChanged signals from firing
    ui_name->blockSignals(true);
    ui_description->blockSignals(true);

    if (debounceCause != ui_name)
      ui_name->document()->setPlainText(evi->name);
    if (debounceCause != ui_description)
      ui_description->document()->setPlainText(evi->description);

    QString imagePath = ao_app->get_evidence_path(evi->image);
    if (!QFile::exists(imagePath))
      imagePath = ao_app->get_evidence_path("empty.png");
    QPixmap icon(imagePath);
    ui_image->setIcon(icon);
    ui_image->setIconSize(icon.size());

    ui_name->blockSignals(false);
    ui_description->blockSignals(false);
  }
  else
  {
    ui_pages->setCurrentWidget(ui_empty_page);
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
  debounceTimer->start();
  qDebug() << "will fire in" << debounceInterval << "ms";
}

void AOEvidence::debounceFired()
{
  debounceTimer->stop();
  debounceTimer->deleteLater();

  if (!selectedEvidence())
    return;

  int id = selectedEvidenceId();
  emit evidenceEdited(id, evidence[id]);
}

void AOEvidence::on_list_currentItemChanged(QListWidgetItem *item)
{
  // Force send changes immediately if debounce timer is running
  if (debounceTimer)
    debounceFired();

  // If item is now null, switch to the empty page
  // Otherwise, switch to the evidence info page and populate entries
  if (item != nullptr && selectedEvidence() != nullptr)
    showEvidence(selectedEvidence());
  else
    ui_pages->setCurrentWidget(ui_empty_page);
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
    newEvidence.image = "empty.png";

    emit evidenceAdded(newEvidence);
  }
}

void AOEvidence::on_name_textChanged()
{
  if (!debounceTimer)
    setupDebouncer();
  debounceCause = ui_name;

  auto *evi = selectedEvidence();
  if (evi == nullptr)
    return;
  evi->name = ui_name->document()->toRawText();
}

void AOEvidence::on_description_textChanged()
{
  if (!debounceTimer)
    setupDebouncer();
  debounceCause = ui_description;

  auto *evi = selectedEvidence();
  if (evi == nullptr)
    return;
  evi->description = ui_description->document()->toRawText();
}

void AOEvidence::on_image_clicked()
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
                          tr("You can only use evidence images that are in the "
                          "evidence folder."));
    return;
  }

  QPixmap icon(evidenceDir.absoluteFilePath(relativePath));
  ui_image->setIcon(icon);
  ui_image->setIconSize(icon.size());

  if (!debounceTimer)
    setupDebouncer();

  evidence[selectedEvidenceId()].image = relativePath;
}

void AOEvidence::on_delete_button_clicked()
{
  if (debounceTimer) {
    debounceTimer->stop();
    debounceTimer->deleteLater();
  }

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
    if (!QFile::exists(imagePath))
      imagePath = ao_app->get_evidence_path("empty.png");
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
    if (!QFile::exists(imagePath))
      imagePath = ao_app->get_evidence_path("empty.png");
    auto item = new QListWidgetItem(QIcon(imagePath), evidence[i].name);
    item->setData(Qt::UserRole, i);
    ui_list->insertItem(i, item);
  }

  // Clear the selection if it no longer exists
  if (originalIndex >= ui_list->count() - 1)
    ui_list->clearSelection();
  else if (selectedEvidence() != nullptr)
    showEvidence(selectedEvidence());

  debounceCause.clear();
}

void AOEvidence::togglePresenting(bool presenting)
{
  ui_present_button->setChecked(presenting);
}
