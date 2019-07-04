#include "widgets/aochat.h"
#include "aouiloader.h"

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

  ui_custom_interjection = findChild<QPushButton *>("custom_interjection");
  ui_holdit = findChild<QPushButton *>("holdit");
  ui_objection = findChild<QPushButton *>("objection");
  ui_takethat = findChild<QPushButton *>("takethat");

  ui_chat_entry = findChild<QTextEdit *>("chat_entry");
  ui_color = findChild<QComboBox *>("color");
  ui_emote = findChild<QComboBox *>("emote");
  ui_side = findChild<QComboBox *>("side");

  ui_flip = findChild<QCheckBox *>("flip");
  ui_no_interrupt = findChild<QCheckBox *>("no_interrupt");
  ui_preanim = findChild<QCheckBox *>("preanim");
  ui_realization = findChild<QCheckBox *>("realization");

  ui_emotes = findChild<QListWidget *>("emotes");

  ui_showname = findChild<QLineEdit *>("showname");

  connect(ui_custom_interjection, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_holdit, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_objection, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
  connect(ui_takethat, &QPushButton::toggled, this, &AOChat::on_interjection_toggled);
}

void AOChat::setCharacter(QString character)
{
  this->setEnabled(!character.isEmpty());

  bool custom = ao_app->custom_objection_enabled &&
      (file_exists(ao_app->get_character_path(character, "custom.gif")) ||
      file_exists(ao_app->get_character_path(character, "custom.apng"))) &&
      file_exists(ao_app->get_character_path(character, "custom.wav"));

  ui_custom_interjection->setVisible(custom);
  window()->setWindowTitle(tr("Character: %1").arg(
                             character.isEmpty() ? "(none)" : character));
}

void AOChat::clearEntry()
{
  ui_chat_entry->clear();
  for (QPushButton * const button : buttons)
    button->setChecked(false);
  ui_preanim->setChecked(false);
  ui_realization->setChecked(false);
}

void AOChat::on_interjection_toggled(bool toggled)
{
  if (toggled)
  {
    auto senderButton = static_cast<QPushButton *>(sender());
    for (QPushButton * const button : buttons)
      if (button != senderButton)
        button->setChecked(false);
  }
}

void AOChat::on_side_currentIndexChanged(int index)
{
  const QVector<QString> positions = {"wit", "def", "pro", "jud", "hld", "hlp", "jur", "sea"};
  emit positionChanged(positions[index]);
}

void AOChat::on_chatEntry_enterPressed()
{
  QString message = ui_chat_entry->document()->toPlainText().trimmed();
  if (message.isEmpty())
    return;

  // TODO
  emit sendMessage();
}
