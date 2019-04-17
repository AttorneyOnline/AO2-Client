#include "aojukebox.h"
#include "aouiloader.h"

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
}

void AOJukebox::on_music_list_doubleClicked(QModelIndex p_model)
{
  QString p_song = track_list.at(p_model.row()).name;

  emit trackSelected(p_song);
}

void AOJukebox::on_search_edited(QString)
{
  refresh();
}

void AOJukebox::refresh()
{
  ui_music_list->clear();
  QString f_file = "courtroom_design.ini";

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  int n_listed_songs = 0;

  for (int n_song = 0 ; n_song < track_list.size() ; ++n_song)
  {
    QString i_song = track_list.at(n_song);
    QString i_song_listname = i_song;
    i_song_listname = i_song_listname.left(i_song_listname.lastIndexOf("."));

    if (i_song.toLower().contains(ui_search->text().toLower()))
    {
      ui_music_list->addItem(i_song_listname);

      QString song_path = ao_app->get_music_path(i_song);

      if (file_exists(song_path))
        ui_music_list->item(n_listed_songs)->setBackground(found_brush);
      else
        ui_music_list->item(n_listed_songs)->setBackground(missing_brush);

      ++n_listed_songs;
    }
  }
}
