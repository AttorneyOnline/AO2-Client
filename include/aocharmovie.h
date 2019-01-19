#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QMovie>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImageReader>
#include <QPainter>
#include "include/aosfxplayer.h"
#include "include/courtroom.h"


class AOApplication;
class AOCharMovie : public QLabel
{
  Q_OBJECT

public:
  AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app);

  void play(QString p_char, QString p_emote, QString emote_prefix);
  void play_pre(QString p_char, QString p_emote, int duration);
  void play_talking(QString p_char, QString p_emote);
  void play_idle(QString p_char, QString p_emote);

  void set_flipped(bool p_flipped) {m_flipped = p_flipped;}
  void LoadImageWithStupidMethodForFlipSupport(QImage image);
  void stop();

  void move(int ax, int ay);
  void combo_resize(int w, int h);
  void play_frame_sfx();

  void sfx_two_network_boogaloo();
  void screenshake_two_network_boogaloo();
  void realization_two_network_boogaloo();

  AOSfxPlayer *frame_specific_sfx_player;
  Courtroom *mycourtroom;
  QString frame_sfx_hellstring = "";
  QString frame_screenshake_hellstring = "";
  QString frame_realization_hellstring = "";
  bool use_networked_framehell = false;
private:
  AOApplication *ao_app;

  QMovie *m_movie;
  QVector<QImage> movie_frames;
  QTimer *preanim_timer;
  QTimer *ticker;
  QString last_path;
  QString current_emote;
  QString current_char;

  const int time_mod = 62;

  // These are the X and Y values before they are fixed based on the sprite's width.
  int x = 0;
  int y = 0;
  int default_w;
  int default_h;

  bool m_flipped = false;

  bool play_once = true;

signals:
  void done();

private slots:
  void timer_done();
  void movie_ticker();
};
#endif // AOCHARMOVIE_H
