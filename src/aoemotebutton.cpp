#include "aoemotebutton.h"

#include "file_functions.h"

#include <QDebug>

AOEmoteButton::AOEmoteButton(int id, int width, int height, AOApplication *ao_app, QWidget *parent)
    : QPushButton(parent)
    , ao_app(ao_app)
    , m_id(id)
{
  resize(width, height);

  ui_selected = new QLabel(this);
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->resize(width, height);
  ui_selected->hide();

  connect(this, &AOEmoteButton::clicked, this, [this] { Q_EMIT emoteClicked(m_id); });
}

void AOEmoteButton::setSelectedImage(QString p_image)
{
  if (file_exists(p_image))
  {
    ui_selected->setStyleSheet("border-image: url(\"" + p_image + "\")");
  }
  else
  {
    ui_selected->setStyleSheet("background-color: rgba(0, 0, 0, 128)");
  }
}

int AOEmoteButton::id()
{
  return m_id;
}

void AOEmoteButton::setImage(QString character, int emoteId, bool enabled)
{
  QString emotion_number = QString::number(emoteId + 1);

  QStringList suffixedPaths;
  static const QStringList SUFFIX_LIST{"_off", "_on"};
  for (const QString &suffix : SUFFIX_LIST)
  {
    suffixedPaths.append(ao_app->get_image_suffix(ao_app->get_character_path(character, "emotions/button" + emotion_number + suffix)));
  }

  QString image = suffixedPaths[static_cast<int>(enabled)];
  if (enabled && !file_exists(suffixedPaths[1]))
  {
    ui_selected->show();
    image = suffixedPaths[0];
  }
  else
  {
    ui_selected->hide();
  }

  if (file_exists(image))
  {
    setText(QString());
    setStyleSheet("QPushButton { border: none; }"
                  "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
    setIcon(QPixmap(image).scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setIconSize(size());
  }
  else
  {
    QString emote_comment = ao_app->get_emote_comment(character, emoteId);
    setText(emote_comment);
    setStyleSheet("QPushButton { border-image: url(); }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
    setIcon(QIcon());
    setIconSize(size());
  }
}
