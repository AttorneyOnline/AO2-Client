#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QImageReader>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImageReader>
#include <QElapsedTimer>
#include <QPainter>
#include "include/aosfxplayer.h"
#include "include/courtroom.h"

class AOApplication;
class AOCharMovie : public QLabel
{
  Q_OBJECT

public:
  AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app);

  //Play a hat.gif - style preanimation
  void play_pre(QString p_char, QString p_emote, int duration);

  //Play a (b)normal.gif - style animation (talking)
  void play_talking(QString p_char, QString p_emote);

  //Play an (a)normal.gif - style animation (not talking)
  void play_idle(QString p_char, QString p_emote);

  //Stop the movie, clearing the image
  void stop();

  //Set the m_flipped variable to true/false
  void set_flipped(bool p_flipped) {m_flipped = p_flipped;}

//Set the movie's playback speed (between 10% and 1000%)
  void set_speed(int modifier) {speed = qMax(10, qMin(modifier, 1000));}

  void LoadImageWithStupidMethodForFlipSupport(QImage image);

  //Move the label itself around
  void move(int ax, int ay);

  //This is somewhat pointless now as there's no "QMovie" object to resize, aka no "combo" to speak of
  void combo_resize(int w, int h);

  //Return the frame delay adjusted for speed
  int get_frame_delay(int delay);

  QStringList network_strings;

  QString m_char;
  QString m_emote;

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

  QVector<QPixmap> movie_frames;
  QVector<int> movie_delays;

  //Effects such as sfx, screenshakes and realization flashes are stored in here.
  //QString entry format: "sfx^[sfx_name]", "shake", "flash".
  //The program uses the QVector index as reference.
  QVector<QVector<QString>> movie_effects;

  QTimer *preanim_timer;
  QTimer *ticker;
  QString last_path;
  QImageReader *m_reader = new QImageReader();
  QString current_emote;
  QString current_char;

  QElapsedTimer actual_time;

  //Usually used to turn seconds into milliseconds such as for [Time] tag in char.ini
  const int time_mod = 60;

  // These are the X and Y values before they are fixed based on the sprite's width.
  int x = 0;
  int y = 0;

  // These are the width and height values before they are fixed based on the sprite's width.
  int f_w = 0;
  int f_h = 0;

  int default_w;
  int default_h;

  int frame = 0;
  int max_frames = 0;

  int speed = 100;

  bool m_flipped = false;
  bool play_once = true;
  bool apng = false;

  //Set the movie's image to provided paths, preparing for playback.
  void load_image(QString p_char, QString p_emote, QString emote_prefix);

  //Start playback of the movie (if animated).
  void play();

  //Play a frame-specific effect, if there's any defined for that specific frame.
  void play_frame_effect(int frame);

  //Retreive a pixmap adjused for mirroring/aspect ratio shenanigans from a provided QImage
  QPixmap get_pixmap(QImage image);

  //Set the movie's frame to provided pixmap
  void set_frame(QPixmap f_pixmap);

  //Initialize the frame-specific effects from the char.ini
  void load_effects();

  //Initialize the frame-specific effects from the provided network_strings, this is only initialized if network_strings has size more than 0.
  void load_network_effects();

signals:
  void done();
  void shake();
  void flash();
  void play_sfx(QString sfx);

private slots:
  void preanim_done();
  void timer_done();
  void movie_ticker();
};
#endif // AOCHARMOVIE_H
