#include "aoevidencedisplay.h"

#include "datatypes.h"
#include "file_functions.h"

AOEvidenceDisplay::AOEvidenceDisplay(AOApplication *p_ao_app, QWidget *p_parent)
    : QLabel(p_parent)
    , ao_app(p_ao_app)
{
  ui_prompt_details = new QPushButton(this);
  ui_prompt_details->hide();

  m_sfx_player = new AOSfxPlayer(ao_app);

  m_evidence_movie = new InterfaceLayer(ao_app, this);

  connect(m_evidence_movie, &InterfaceLayer::done, this, &AOEvidenceDisplay::show_done);
  connect(ui_prompt_details, &QPushButton::clicked, this, &AOEvidenceDisplay::icon_clicked);
}

void AOEvidenceDisplay::show_evidence(int p_index, QString p_evidence_image, bool is_left_side, int p_volume)
{
  this->reset();

  m_last_evidence_index = p_index;

  m_sfx_player->setVolume(p_volume);

  QString gif_name;
  QString icon_identifier;

  if (is_left_side)
  {
    icon_identifier = "left_evidence_icon";
    gif_name = "evidence_appear_left";
  }
  else
  {
    icon_identifier = "right_evidence_icon";
    gif_name = "evidence_appear_right";
  }

  QString f_evidence_path = ao_app->get_real_path(ao_app->get_evidence_path(p_evidence_image));
  QPixmap f_pixmap(f_evidence_path);

  pos_size_type icon_dimensions = ao_app->get_element_dimensions(icon_identifier, "courtroom_design.ini");

  f_pixmap = f_pixmap.scaled(icon_dimensions.width, icon_dimensions.height);
  QIcon f_icon(f_pixmap);

  ui_prompt_details->setIcon(f_icon);
  ui_prompt_details->setIconSize(f_pixmap.rect().size());
  ui_prompt_details->resize(f_pixmap.rect().size());
  ui_prompt_details->move(icon_dimensions.x, icon_dimensions.y);
  m_evidence_movie->static_duration = 320;
  m_evidence_movie->max_duration = 1000;
  m_evidence_movie->set_play_once(true);
  m_evidence_movie->load_image(gif_name, "");
  m_sfx_player->findAndPlaySfx(ao_app->get_court_sfx("evidence_present"));
}

void AOEvidenceDisplay::reset()
{
  m_sfx_player->stop();
  m_evidence_movie->kill();
  ui_prompt_details->hide();
  this->clear();
}

void AOEvidenceDisplay::show_done()
{
  ui_prompt_details->show();
}

void AOEvidenceDisplay::icon_clicked()
{
  if (m_last_evidence_index != -1)
  {
    Q_EMIT show_evidence_details(m_last_evidence_index - 1); // i dont know why i have to subtract 1 here
  }
}

void AOEvidenceDisplay::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_evidence_movie->combo_resize(w, h);
}
