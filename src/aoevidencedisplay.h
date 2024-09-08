#pragma once

#include "animationlayer.h"
#include "aoapplication.h"
#include "aosfxplayer.h"

#include <QDebug>
#include <QLabel>
#include <QPushButton>

class AOEvidenceDisplay : public QLabel
{
  Q_OBJECT

public:
  AOEvidenceDisplay(AOApplication *p_ao_app, QWidget *p_parent = nullptr);

  void show_evidence(int p_index, QString p_evidence_image, bool is_left_side, int p_volume);
  void reset();
  void combo_resize(int w, int h);
  void setLastEvidenceIndex(int f_index);

Q_SIGNALS:
  void show_evidence_details(int index);

private:
  AOApplication *ao_app;

  int m_last_evidence_index = -1;
  AOSfxPlayer *m_sfx_player;

  kal::InterfaceAnimationLayer *m_evidence_movie;
  QPushButton *ui_prompt_details;

private Q_SLOTS:
  void show_done();
  void icon_clicked();
};
