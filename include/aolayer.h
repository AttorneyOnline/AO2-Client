#ifndef AOLAYER_H
#define AOLAYER_H

#include <QDebug>
#include <QElapsedTimer>
#include <QImageReader>
#include <QLabel>
#include <QTimer>

class AOApplication;

class AOLayer : public QLabel {
  Q_OBJECT

public:
  AOLayer(QWidget *p_parent, AOApplication *p_ao_app);

  enum class LayerType {
    background,   // the background of the scene
    foreground,   // for effects meant to appear under the character i.e.
                  // speedlines
    character,    // for characters
    interjection, // for shouts, e.g. Objection!
    effect,       // effects, like flashes
    ui,           // for animated UI elements, i.e. chat_arrow
  };

  struct LayerProperties {
    LayerType type;   // Type of layer, explained above
    QString filename; // file name without extension, i.e. "witnesstestimony"
    QString charname; // name of the character folder to search, if applicable
    QString miscname; // name of the misc folder to search, if applicable
    int static_duration; // time in ms for static images to be displayed, if
                         // applicable
    int max_duration; // maximum duration in ms, image will be culled if it is
                      // exceeded. set this to 0 for infinite duration
  };

  // Play a hat.gif - style preanimation
  void play_pre(QString p_char, QString p_emote, int duration);

  // Set the movie's image to provided paths, preparing for playback.
  void load_image(LayerProperties p_props);

  void set_play_once(bool p_play_once);

  // Play a (b)normal.gif - style animation (talking)
  void play_talking(QString p_char, QString p_emote);

  // Play an (a)normal.gif - style animation (not talking)
  void play_idle(QString p_char, QString p_emote);

  // Stop the movie, clearing the image
  void stop();

  // Set the m_flipped variable to true/false
  void set_flipped(bool p_flipped) { m_flipped = p_flipped; }

  // Set the movie's playback speed (between 10% and 1000%)
  void set_speed(int modifier) { speed = qMax(10, qMin(modifier, 1000)); }

  // Move the label itself around
  void move(int ax, int ay);

  // This is somewhat pointless now as there's no "QMovie" object to resize, aka
  // no "combo" to speak of
  void combo_resize(int w, int h);

  // Return the frame delay adjusted for speed
  int get_frame_delay(int delay);

  // networked frame fx nonsense
  QStringList network_strings;

private:
  AOApplication *ao_app;

  QVector<QPixmap> movie_frames;
  QVector<int> movie_delays;

  // Effects such as sfx, screenshakes and realization flashes are stored in
  // here. QString entry format: "sfx^[sfx_name]", "shake", "flash". The program
  // uses the QVector index as reference.
  QVector<QVector<QString>> movie_effects;

  QTimer *preanim_timer;
  QTimer *shfx_timer;
  QTimer *ticker;
  QString last_path;
  QImageReader *m_reader;

  QElapsedTimer actual_time;

  QString m_char;
  QString m_emote;

  // Usually used to turn seconds into milliseconds such as for [Time] tag in
  // char.ini
  const int tick_ms = 60;

  // These are the X and Y values before they are fixed based on the sprite's
  // width.
  int x = 0;
  int y = 0;
  // These are the width and height values before they are fixed based on the
  // sprite's width.
  int f_w = 0;
  int f_h = 0;

  int frame = 0;
  int max_frames = 0;

  int speed = 100;

  bool m_flipped = false;
  bool play_once = false;
  bool is_effect = false;

  int duration = 0;

  // Start playback of the movie (if animated).
  void play();

  // Freeze the movie at the current frame.
  void freeze();

  // Play a frame-specific effect, if there's any defined for that specific
  // frame.
  void play_frame_effect(int frame);

  // Retreive a pixmap adjused for mirroring/aspect ratio shenanigans from a
  // provided QImage
  QPixmap get_pixmap(QImage image);

  // Set the movie's frame to provided pixmap
  void set_frame(QPixmap f_pixmap);

  // Initialize the frame-specific effects from the char.ini
  void load_effects();

  // Initialize the frame-specific effects from the provided network_strings,
  // this is only initialized if network_strings has size more than 0.
  void load_network_effects();

signals:
  void done();
  void shake();
  void flash();
  void play_sfx(QString sfx);

private slots:
  void preanim_done();
  void shfx_timer_done();
  void movie_ticker();
};

#endif // AOLAYER_H
