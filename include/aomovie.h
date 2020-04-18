#ifndef AOMOVIE_H
#define AOMOVIE_H

#include <QLabel>
#include <QMovie>

class Courtroom;
class AOApplication;

class AOMovie : public QLabel {
  Q_OBJECT

public:
  AOMovie(QWidget *p_parent, AOApplication *p_ao_app);

  void set_play_once(bool p_play_once);
  void start_timer(int delay);
  void play(QString p_gif, QString p_char = "", QString p_custom_theme = "",
            int default_duration = 0);
  void combo_resize(int w, int h);
  void stop();

private:
  QMovie *m_movie;
  AOApplication *ao_app;
  QTimer *timer;
  bool play_once = true;

signals:
  void done();

private slots:
  void frame_change(int n_frame);
  void timer_done();
};

#endif // AOMOVIE_H
