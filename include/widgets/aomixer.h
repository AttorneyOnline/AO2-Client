#ifndef AOMIXER_H
#define AOMIXER_H

#include "aoapplication.h"

#include <QProgressBar>
#include <QSlider>
#include <QWidget>

class AOMixer : public QWidget
{
  Q_OBJECT
public:
  explicit AOMixer(QWidget *parent, AOApplication *p_ao_app);

private:
  AOApplication *ao_app;

  QProgressBar *ui_master_level;
  QSlider *ui_music_slider;
  QSlider *ui_sfx_slider;
  QSlider *ui_blips_slider;

signals:
  void volumeChanged(AUDIO_TYPE type, int volume);

public slots:
  // Level should be normalized from 0-100%.
  void set_master_level(int level);

private slots:
  void on_musicSlider_valueChanged(int volume);
  void on_sfxSlider_valueChanged(int volume);
  void on_blipsSlider_valueChanged(int volume);
};

#endif // AOMIXER_H
