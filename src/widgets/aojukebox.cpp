#include "widgets/aojukebox.h"
#include "aouiloader.h"
#include "file_functions.h"

#include <QVBoxLayout>

AOJukebox::AOJukebox(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/jukebox.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_music_list = findChild<QListWidget *>("music_list");
  ui_search = findChild<QLineEdit *>("search");
}

void AOJukebox::set_tracks(QVector<track_type> &tracks)
{
  track_list = tracks;
  refresh();
}

void AOJukebox::on_music_list_doubleClicked(QModelIndex p_model)
{
  QString p_song = track_list.at(p_model.row()).name;

  emit trackSelected(p_song);
}

void AOJukebox::on_search_textEdited(QString)
{
  refresh();
}

void AOJukebox::refresh()
{
  ui_music_list->clear();
  QString f_file = "courtroom_design.ini";

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  for (const track_type &track : track_list)
  {
    QString i_song_listname = track.name;
    i_song_listname = i_song_listname.left(i_song_listname.lastIndexOf("."));

    if (track.name.toLower().contains(ui_search->text().toLower()))
    {
      auto entry = new QListWidgetItem(i_song_listname, ui_music_list);
      ui_music_list->addItem(entry);

      QString song_path = ao_app->get_music_path(track.name);

      if (file_exists(song_path))
        entry->setBackground(found_brush);
      else
        entry->setBackground(missing_brush);
    }
  }
}
