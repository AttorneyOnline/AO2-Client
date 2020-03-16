#include "widgets/aochat.h"
#include "aouiloader.h"

#include "file_functions.h"

#include <QVBoxLayout>

AOChat::AOChat(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/ic.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QStackedWidget, stacked_widget)
  FROM_UI(QWidget, page_normal)
  FROM_UI(QWidget, page_spectator)
  FROM_UI(QPushButton, custom_interjection)
  FROM_UI(QPushButton, holdit)
  FROM_UI(QPushButton, objection)
  FROM_UI(QPushButton, takethat)
  FROM_UI(QTextEdit, chat_entry)
  FROM_UI(QComboBox, color)
  FROM_UI(QComboBox, emote)
  FROM_UI(QComboBox, side)
  FROM_UI(QCheckBox, flip)
  FROM_UI(QCheckBox, no_interrupt)
  FROM_UI(QCheckBox, preanim)
  FROM_UI(QCheckBox, realization)
  FROM_UI(QListWidget, emotes)
  FROM_UI(QLineEdit, showname)

  connect(ui_custom_interjection, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_holdit, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_objection, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_takethat, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
}

void AOChat::setCharacter(const QString &character)
{
  this->character = character;
  this->setEnabled(!character.isEmpty());

  if (windowFlags() & Qt::Tool)
    window()->setWindowTitle(tr("Character: %1").arg(
                               character.isEmpty() ? "(none)" : character));

  if (character.isEmpty())
  {
    ui_stacked_widget->setCurrentWidget(ui_page_spectator);
    return;
  }

  ui_stacked_widget->setCurrentWidget(ui_page_normal);

  // Show custom button
  const bool custom =
      (file_exists(ao_app->get_character_path(character, "custom.gif")) ||
      file_exists(ao_app->get_character_path(character, "custom.apng"))) &&
      file_exists(ao_app->get_character_path(character, "custom.wav"));

  ui_custom_interjection->setVisible(custom);
  ui_custom_interjection->setChecked(false);

  // Populate emote lists
  ui_emotes->clear();
  ui_emote->clear();

  const int totalEmotes = ao_app->get_emote_number(character);
  for (int i = 0; i < totalEmotes; i++)
  {
    const QString imagePath = QStringLiteral("emotions/button%1%2").arg(i);
    const QString imagePathOff = ao_app->get_character_path(character, imagePath.arg("_off.png"));
    const QString imagePathOn = ao_app->get_character_path(character, imagePath.arg("_on.png"));
    const QString emoteName = ao_app->get_emote_comment(character, i);

    auto emoteButton = new QListWidgetItem(emoteName);

    QIcon icon;
    icon.addFile(imagePathOff, QSize(), QIcon::Mode::Normal);
    icon.addFile(imagePathOn, QSize(), QIcon::Mode::Selected);
    emoteButton->setIcon(icon);
    emoteButton->setData(Qt::UserRole, i);

    ui_emotes->addItem(emoteButton);
    ui_emote->addItem(emoteName, i);
  }
}

void AOChat::clearEntry()
{
  ui_chat_entry->clear();
  for (QPushButton * const button : interjectionButtons)
    button->setChecked(false);
  ui_preanim->setChecked(false);
  ui_realization->setChecked(false);
}

void AOChat::on_interjection_toggled(bool toggled)
{
  if (toggled)
  {
    auto senderButton = static_cast<QPushButton *>(sender());
    for (QPushButton * const button : interjectionButtons)
      if (button != senderButton)
        button->setChecked(false);
  }
}

void AOChat::on_side_currentIndexChanged(int index)
{
  emit positionChanged(positions[index]);
}

void AOChat::on_emote_currentIndexChanged(int index)
{
  ui_emotes->setCurrentRow(index);
}

void AOChat::on_emotes_currentItemChanged(QListWidgetItem *cur, QListWidgetItem *prev)
{
  // Stop trying to deselect the emote!!
  if (cur == nullptr && prev != nullptr)
  {
    ui_emotes->setCurrentItem(prev);
    return;
  }
  else if (cur == nullptr)
  {
    // This case should never happen
    qWarning() << "impossible case occurred";
    return;
  }

  int emote = cur->data(Qt::UserRole).toInt();
  ui_emote->setCurrentIndex(emote);

  EMOTE_MODIFIER modifier = ao_app->get_emote_mod(character, emote);
  ui_preanim->setChecked(modifier == PREANIM);
}

void AOChat::on_emotes_itemClicked(QListWidgetItem *emote)
{
  if (emote == nullptr)
    return;

  // This is probably not going to work the way that I expected because
  // this depends on the order in which order itemClicked and
  // currentItemChanged are handled. If this slot is handled too late,
  // then this condition will always be true.
  if (emote->data(Qt::UserRole) == ui_emote->currentData())
    ui_preanim->toggle();
}

void AOChat::on_chatEntry_enterPressed()
{
  QString message = ui_chat_entry->document()->toPlainText().trimmed();
  if (message.isEmpty())
    return;

  emit messageSent();
}

void AOChat::addMessageData(chat_message_type &message)
{
  if (ui_emotes->selectedItems().size() == 0)
  {
    // Stop trying to break the game! I told you to stop!!
    return;
  }

  // Get the index of the first selected emote
  int emote = ui_emotes->selectedItems()[0]->data(Qt::UserRole).toInt();

  message.desk = ao_app->get_desk_mod(character, emote);
  message.preanim = ao_app->get_pre_emote(character, emote);
  message.character = character;
  message.anim = ao_app->get_emote(character, emote);
  message.message = ui_chat_entry->document()->toPlainText();
  message.side = positions[ui_side->currentIndex()];
  message.sfx_name = ao_app->get_sfx_name(character, emote);
  message.emote_modifier = ao_app->get_emote_mod(character, emote);

  // Char ID not set here because this class doesn't have access to it.

  message.sfx_delay = ao_app->get_sfx_delay(character, emote);

  if (ui_objection->isChecked())
    message.objection_modifier = OBJECTION;
  else if (ui_holdit->isChecked())
    message.objection_modifier = HOLD_IT;
  else if (ui_takethat->isChecked())
    message.objection_modifier = TAKE_THAT;
  else if (ui_custom_interjection->isChecked())
    message.objection_modifier = CUSTOM;
  else
    message.objection_modifier = NONE;

  // No access to evidence info.

  message.flip = ui_flip->isChecked();
  message.realization = ui_realization->isChecked();
  message.text_color = static_cast<COLOR>(ui_color->currentIndex());
  message.showname = ui_showname->text();

  // No access to pair info.

  message.noninterrupting_preanim = ui_no_interrupt->isChecked();
}

bool AOChat::interjectionSelected()
{
  for (QPushButton * const button : interjectionButtons)
    if (button->isChecked())
      return true;
  return false;
}
