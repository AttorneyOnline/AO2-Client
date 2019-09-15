#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QImageReader>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImageReader>
#include <QElapsedTimer>

class AOApplication;

class AOCharMovie : public QLabel
{
  Q_OBJECT

public:
  AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app);

  //Set the movie's image to provided paths, preparing for playback.
  void load_image(QString p_char, QString p_emote, QString emote_prefix);

  //Start playback of the movie (if animated).
  void play();

  //Play a hat.gif - style preanimation
  void play_pre(QString p_char, QString p_emote, int duration);

  //Play a (b)normal.gif - style animation (talking)
  void play_talking(QString p_char, QString p_emote);

  //Play an (a)normal.gif - style animation (not talking)
  void play_idle(QString p_char, QString p_emote);

  //Retreive a pixmap adjused for mirroring/aspect ratio shenanigans from a provided QImage
  QPixmap get_pixmap(QImage image);

  //Set the movie's frame to provided pixmap
  void set_frame(QPixmap f_pixmap);

  //Stop the movie, clearing the image
  void stop();

  //Set the m_flipped variable to true/false
  void set_flipped(bool p_flipped) {m_flipped = p_flipped;}

  //Set the movie's playback speed (between 10% and 1000%)
  void set_speed(int modifier) {speed = qMax(10, qMin(modifier, 1000));}

  //Move the label itself around
  void move(int ax, int ay);

  //This is somewhat pointless now as there's no "QMovie" object to resize, aka no "combo" to speak of
  void combo_resize(int w, int h);

  //Return the frame delay adjusted for speed
  int get_frame_delay(int delay);

private:
  AOApplication *ao_app;

  QVector<QPixmap> movie_frames;
  QVector<int> movie_delays;
  QTimer *preanim_timer;
  QTimer *ticker;
  QString last_path;
  QImageReader *m_reader = new QImageReader();

  QElapsedTimer actual_time;
  const int time_mod = 60;

  // These are the X and Y values before they are fixed based on the sprite's width.
  int x = 0;
  int y = 0;

  int frame = 0;
  int max_frames = 0;

  int speed = 100;

  bool m_flipped = false;
  bool play_once = true;

signals:
  void done();

private slots:
  void preanim_done();
  void movie_ticker();
};

#endif // AOCHARMOVIE_H
