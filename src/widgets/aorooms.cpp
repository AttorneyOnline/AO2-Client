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

  setWindowTitle(tr("Room List"));

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->setMargin(0);
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  FROM_UI(QListWidget, area_list)
  FROM_UI(QLineEdit, search)

  connect(ui_area_list, &QListWidget::doubleClicked,
          this, &AORoomChooser::on_area_list_doubleClicked);
  connect(ui_search, &QLineEdit::textEdited,
          this, &AORoomChooser::on_search_textEdited);
}

void AORoomChooser::setAreas(QVector<area_type> areas)
{
  area_list = areas;
  refresh();
}

void AORoomChooser::on_area_list_doubleClicked(QModelIndex p_model)
{
  if (p_model.row() >= area_list.length()) {
    qWarning() << "selected area is out of area list bounds";
    return;
  }

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

  for (int i = 0 ; i < area_list.size() ; ++i)
  {
    const area_type &area = area_list.at(i);
    const QString desc = tr("[%1] %2\n"
                            "  %3 | CM: %4\n"
                            "  %5 users | %6")
        .arg(i)
        .arg(area.name)
        .arg(area.status)
        .arg(area.cm)
        .arg(area.players)
        .arg(area.locked);

    if (desc.toLower().contains(ui_search->text().toLower()))
    {
      auto area_item = new QListWidgetItem(desc, ui_area_list);

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
  }
}
