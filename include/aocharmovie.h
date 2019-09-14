#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QImageReader>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QImageReader>

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

  void set_frame(QImage image);

  void stop();

  void set_flipped(bool p_flipped) {m_flipped = p_flipped;}

  void move(int ax, int ay);

  void combo_resize(int w, int h);


private:
  AOApplication *ao_app;

  QImageReader *m_reader;
  QTimer *preanim_timer;
  QTimer *ticker;
  QString last_path;

  const int time_mod = 60;

  // These are the X and Y values before they are fixed based on the sprite's width.
  int x = 0;
  int y = 0;

  bool m_flipped = false;

  bool play_once = true;

signals:
  void done();

private slots:
  void preanim_done();
  void movie_ticker();
};

#endif // AOCHARMOVIE_H
