#ifndef AOEVIDENCEDISPLAY_H
#define AOEVIDENCEDISPLAY_H

#include "aoapplication.h"
#include "aolayer.h"
#include "aosfxplayer.h"

#include <QDebug>
#include <QLabel>
#include <QPushButton>

class AOEvidenceDisplay : public QLabel {
  Q_OBJECT

public:
  AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app);

  void show_evidence(int p_index, QString p_evidence_image, bool is_left_side, int p_volume);
  void reset();
  void combo_resize(int w, int h);

  int last_evidence_index = -1;
signals:
  void show_evidence_details(int index);

private:
  AOApplication *ao_app;
  InterfaceLayer *evidence_movie;
  QPushButton *evidence_icon;
  AOSfxPlayer *sfx_player;

private slots:
  void show_done();
  void icon_clicked();
};

#endif // AOEVIDENCEDISPLAY_H
