#include "widgets/aorooms.h"
#include "aouiloader.h"
#include "file_functions.h"

#include <QVBoxLayout>

AORoomChooser::AORoomChooser(QWidget *parent, AOApplication *p_ao_app)
  : QWidget(parent), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/rooms.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  ui_area_list = findChild<QListWidget *>("area_list");
  ui_search = findChild<QLineEdit *>("search");
}

void AORoomChooser::set_areas(QVector<area_type> &areas)
{
  area_list = areas;
  refresh();
}

void AORoomChooser::on_area_list_doubleClicked(QModelIndex p_model)
{
  QString p_room = area_list.at(p_model.row()).name;

  emit roomSelected(p_room);
}

void AORoomChooser::on_search_textEdited(QString)
{
  refresh();
}

void AORoomChooser::refresh()
{
  ui_area_list->clear();

  QString f_file = "courtroom_design.ini";

  QBrush free_brush(ao_app->get_color("area_free_color", f_file));
  QBrush lfp_brush(ao_app->get_color("area_lfp_color", f_file));
  QBrush casing_brush(ao_app->get_color("area_casing_color", f_file));
  QBrush recess_brush(ao_app->get_color("area_recess_color", f_file));
  QBrush rp_brush(ao_app->get_color("area_rp_color", f_file));
  QBrush gaming_brush(ao_app->get_color("area_gaming_color", f_file));
  QBrush locked_brush(ao_app->get_color("area_locked_color", f_file));

  for (int n_area = 0 ; n_area < area_list.size() ; ++n_area)
  {
    auto area = area_list.at(n_area);
    QString i_area = "";
    i_area.append("[");
    i_area.append(QString::number(n_area));
    i_area.append("] ");

    i_area.append(area.name);

    if (ao_app->arup_enabled)
    {
      i_area.append("\n  ");

      i_area.append(area.status);
      i_area.append(" | CM: ");
      i_area.append(area.cm);

      i_area.append("\n  ");

      i_area.append(QString::number(area.players));
      i_area.append(" users | ");

      i_area.append(area.locked);
    }

    if (i_area.toLower().contains(ui_search->text().toLower()))
    {
      auto area_item = new QListWidgetItem(i_area, ui_area_list);
      ui_area_list->addItem(i_area);

      if (ao_app->arup_enabled)
      {
        // Colouring logic here.
        const std::map<QString, QBrush &> backgrounds = {
          {"LOOKING-FOR-PLAYERS", lfp_brush},
          {"CASING", casing_brush},
          {"RECESS", recess_brush},
          {"RP", rp_brush},
          {"GAMING", gaming_brush},
          {"FREE", free_brush}
        };

        QBrush *brush;
        if (area.locked == "LOCKED")
          brush = &locked_brush;
        else if (backgrounds.count(area.status) > 0)
          brush = &backgrounds.at(area.status);
        else
          brush = &free_brush;

        area_item->setBackground(*brush);
      }
      else
      {
        area_item->setBackground(free_brush);
      }
    }
  }
}
