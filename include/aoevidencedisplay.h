#ifndef AOEVIDENCEDISPLAY_H
#define AOEVIDENCEDISPLAY_H

#include "aoapplication.h"
#include "aosfxplayer.h"
#include "aomovie.h"

#include <QLabel>
#include <QDebug>

class AOEvidenceDisplay : public QLabel
{
  Q_OBJECT

public:
  AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app);

  void show_evidence(QString p_evidence_image, bool is_left_side, int p_volume);
  QLabel* get_evidence_icon();
  void reset();
  void combo_resize(int w, int h);

private:
  AOApplication *ao_app;
  AOMovie *evidence_movie;
  QLabel *evidence_icon;
  AOSfxPlayer *sfx_player;

private slots:
  void show_done();
};

#endif // AOEVIDENCEDISPLAY_H
