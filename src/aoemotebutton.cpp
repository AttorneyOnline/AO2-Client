#include "aoemotebutton.h"
#include "file_functions.h"

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app,
                             int p_x, int p_y, int p_w, int p_h)
    : QPushButton(p_parent)
{
  parent = p_parent;
  ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize(p_w, p_h);

  connect(this, &AOEmoteButton::clicked, this, &AOEmoteButton::on_clicked);
}

void AOEmoteButton::set_image(QString p_image, QString p_emote_comment)
{
  QString tmp_p_image = p_image;

  if (file_exists(p_image)) {
    this->setText("");
    this->setStyleSheet(
        "QPushButton { border-image: url(\"" + p_image +
        "\") 0 0 0 0 stretch stretch; }"
        "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
  }
  else {
    this->setText(p_emote_comment);
    this->setStyleSheet("QPushButton { border-image: url(); }"
                        "QToolTip { background-image: url(); color: #000000; "
                        "background-color: #ffffff; border: 0px; }");
  }
}

void AOEmoteButton::set_char_image(QString p_char, int p_emote, bool on)
{
  QString emotion_number = QString::number(p_emote + 1);
  QStringList suffixes { "_off", "_on" };
  QStringList suffixedPaths;
  for (const QString &suffix : suffixes) {
    suffixedPaths.append(ao_app->get_image_suffix(ao_app->get_character_path(
        p_char, "emotions/button" + emotion_number + suffix)));
  }

  QString emoteComment = ao_app->get_emote_comment(p_char, p_emote);
  if (!file_exists(suffixedPaths[on]) && file_exists(suffixedPaths[!on])) {
    QImage tmpImage(suffixedPaths[!on]);
    tmpImage = tmpImage.convertToFormat(QImage::Format_ARGB32);
    QPoint p1, p2;
    p2.setY(tmpImage.height());

    QLinearGradient gradient(p1, p2);
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(1, QColor(0, 0, 0, 159));

    QPainter p(&tmpImage);
    p.fillRect(0, 0, tmpImage.width(), tmpImage.height(), gradient);

    gradient.setColorAt(0, QColor(0, 0, 0, 159));
    gradient.setColorAt(1, Qt::transparent);
    p.fillRect(0, 0, tmpImage.width(), tmpImage.height(), gradient);

    p.end();

    // Original suffixed path may be empty, so create the path again
    suffixedPaths[on] = QString(suffixedPaths[!on]).replace(suffixes[!on], suffixes[on]);
    tmpImage.save(suffixedPaths[on], "png");
  }

  set_image(suffixedPaths[on], emoteComment);
}

void AOEmoteButton::on_clicked() { emit emote_clicked(m_id); }
