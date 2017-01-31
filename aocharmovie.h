#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QMovie>
#include <QLabel>
#include <QTimer>

class AOApplication;

class AOCharMovie : public QLabel
{
  Q_OBJECT

public:
  AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app);

  void set(QString p_char, QString p_pre, QString p_gif);

  void play_pre();
  void play_talking();
  void play_idle();

  void stop();

  void combo_resize(int w, int h);

private:
  AOApplication *ao_app;

  bool play_once = true;

  QMovie *m_movie;
  QTimer *m_timer;

  QString m_char = "null";
  QString m_pre;
  QString m_gif;

signals:
  void done();

private slots:
  void frame_change(int n_frame);
};

#endif // AOCHARMOVIE_H
