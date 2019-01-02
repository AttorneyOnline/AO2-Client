#ifndef AOMOVIE_H
#define AOMOVIE_H

#include <QLabel>
#include <QMovie>

class Courtroom;
class AOApplication;

class AOMovie : public QLabel
{
  Q_OBJECT

public:
  AOMovie(QWidget *p_parent, AOApplication *p_ao_app);

  void set_play_once(bool p_play_once);
  void play(QString p_gif, QString p_char = "", QString p_custom_theme = "");
  void combo_resize(int w, int h);
  void stop();

private:
  QMovie *m_movie;
  AOApplication *ao_app;
  bool play_once = true;

signals:
  void done();

private slots:
  void frame_change(int n_frame);
};

#endif // AOMOVIE_H
