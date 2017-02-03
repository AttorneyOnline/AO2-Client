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

  void play_pre(QString p_char, QString p_emote, int duration);
  void play_talking(QString p_char, QString p_emote);
  void play_idle(QString p_char, QString p_emote);

  void stop();

  void combo_resize(int w, int h);

private:
  AOApplication *ao_app;

  QMovie *m_movie;
  QTimer *preanim_timer;

signals:
  void done();

private slots:
  void frame_change(int n_frame);
  void preanim_done();
};

#endif // AOCHARMOVIE_H
