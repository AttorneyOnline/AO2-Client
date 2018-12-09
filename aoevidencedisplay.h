#ifndef AOEVIDENCEDISPLAY_H
#define AOEVIDENCEDISPLAY_H

#include "aosfxplayer.h"

#include <QLabel>
#include <QMovie>
#include <QDebug>

class AOEvidenceDisplay : public QLabel
{
  Q_OBJECT

public:
  AOEvidenceDisplay(QWidget *p_parent);

  void show_evidence(QString p_evidence_image, bool is_left_side, int p_volume);
  QLabel* get_evidence_icon();
  void reset();

private:
  QMovie *evidence_movie;
  QLabel *evidence_icon;
  AOSfxPlayer *sfx_player;

private slots:
  void frame_change(int p_frame);
};

#endif // AOEVIDENCEDISPLAY_H
