#include "aoevidencedisplay.h"

#include "datatypes.h"
#include "file_functions.h"
#include "misc_functions.h"

AOEvidenceDisplay::AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app)
    : QLabel(p_parent)
{
  ao_app = p_ao_app;
  evidence_icon = new QPushButton(this);
  evidence_icon->hide();
  sfx_player = new AOSfxPlayer(this, ao_app);

  evidence_movie = new InterfaceLayer(this, ao_app);

  connect(evidence_movie, &InterfaceLayer::done, this, &AOEvidenceDisplay::show_done);
  connect(evidence_icon, &QPushButton::clicked, this, &AOEvidenceDisplay::icon_clicked);
}

void AOEvidenceDisplay::show_evidence(int p_index, QString p_evidence_image,
                                      bool is_left_side, int p_volume)
{
  this->reset();

  last_evidence_index = p_index;

  sfx_player->set_volume(p_volume);

  QString gif_name;
  QString icon_identifier;

  if (is_left_side) {
    icon_identifier = "left_evidence_icon";
    gif_name = "evidence_appear_left";
  }
  else {
    icon_identifier = "right_evidence_icon";
    gif_name = "evidence_appear_right";
  }

  QString f_evidence_path = ao_app->get_real_path(
        ao_app->get_evidence_path(p_evidence_image));
  QPixmap f_pixmap(f_evidence_path);

  pos_size_type icon_dimensions =
      ao_app->get_element_dimensions(icon_identifier, "courtroom_design.ini");

  f_pixmap = f_pixmap.scaled(icon_dimensions.width, icon_dimensions.height);
  QIcon f_icon(f_pixmap);

  evidence_icon->setIcon(f_icon);
  evidence_icon->setIconSize(f_pixmap.rect().size());
  evidence_icon->resize(f_pixmap.rect().size());
  evidence_icon->move(icon_dimensions.x, icon_dimensions.y);
  evidence_movie->static_duration = 320;
  evidence_movie->max_duration = 1000;
  evidence_movie->set_play_once(true);
  evidence_movie->load_image(gif_name, "");
  sfx_player->play(ao_app->get_court_sfx("evidence_present"));
}

void AOEvidenceDisplay::reset()
{
  sfx_player->stop();
  evidence_movie->kill();
  evidence_icon->hide();
  this->clear();
  last_evidence_index = -1;
}

void AOEvidenceDisplay::show_done() { evidence_icon->show(); }

void AOEvidenceDisplay::icon_clicked() {
  if (last_evidence_index != -1) {
    emit show_evidence_details(last_evidence_index - 1); // i dont know why i have to subtract 1 here
  }
}

void AOEvidenceDisplay::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  evidence_movie->combo_resize(w, h);
}
