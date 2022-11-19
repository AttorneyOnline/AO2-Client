#ifndef AOLAYER_H
#define AOLAYER_H

#include <QDebug>
#include <QElapsedTimer>
#include <QImageReader>
#include <QLabel>
#include <QTimer>
#include <QBitmap>
#include <QtConcurrent/QtConcurrentRun>
#include <QMutex>
#include <QWaitCondition>

class AOApplication;
class VPath;

// "Brief" explanation of what the hell this is:
//
// AOLayer handles all animations both inside and outside
// the viewport. It was originally devised as a layering
// system, but turned into a full refactor of the existing
// animation code. 
//
// AOLayer has six subclasses, all of which differ mainly in
// how they handle path resolution.
//
//  - BackgroundLayer: self-explanatory, handles files found in base/background
//  - CharLayer: handles all the "wonderful" quirks of character path resolution 
//  - SplashLayer: handles elements that can either be provided by a misc/ directory
//    or by the theme - speedlines, shouts, WT/CE, et cetera
//  - EffectLayer: this is basically a dummy layer since effects do their own wonky
//    path resolution in a different file
//  - InterfaceLayer: handles UI elements like the chat arrow and the music display
//  - StickerLayer: Crystalwarrior really wanted this. Handles "stickers," whatever those are.
//
// For questions comments or concerns, bother someone else

class AOLayer : public QLabel {
  Q_OBJECT

public:
  AOLayer(QWidget *p_parent, AOApplication *p_ao_app);

  QString filename;    // file name without extension, i.e. "witnesstestimony"
  int static_duration; // time in ms for static images to be displayed, if
                       // applicable. set to 0 for infinite
  int max_duration;    // maximum duration in ms, image will be culled if it is
                       // exceeded. set this to 0 for infinite duration
  bool play_once = false; // Whether to loop this animation or not
  bool cull_image = true; // if we're done playing this animation, should we
                          // hide it? also controls durational culling
  // Are we loading this from the same frame we left off on? 
  bool continuous = false;
  // Whether or not to forcibly bypass the simple check done by start_playback
  // and use the existent value of continuous instead
  bool force_continuous = false;
  Qt::TransformationMode transform_mode = Qt::FastTransformation; // transformation mode to use for this image
  bool stretch = false; // Should we stretch/squash this image to fill the screen?
  bool masked = true; // Set a mask to the dimensions of the widget?

  // Set the movie's image to provided paths, preparing for playback.
  void start_playback(QString p_image);

  void set_play_once(bool p_play_once);
  void set_cull_image(bool p_cull_image);
  void set_static_duration(int p_static_duration);
  void set_max_duration(int p_max_duration);

  // Stop the movie, clearing the image
  void stop();

  // Stop the movie and clear all vectors
  void kill();

  // Set the m_flipped variable to true/false
  void set_flipped(bool p_flipped) { m_flipped = p_flipped; }

  // Set the movie's playback speed (between 10% and 1000%)
  void set_speed(int modifier) { speed = qMax(10, qMin(modifier, 1000)); }

  // Move the label itself around
  void move(int ax, int ay);

  // Move the label and center it
  void move_and_center(int ax, int ay);

  // This is somewhat pointless now as there's no "QMovie" object to resize, aka
  // no "combo" to speak of
  void combo_resize(int w, int h);

  // Return the frame delay adjusted for speed
  int get_frame_delay(int delay);

  // iterate through a list of paths and return the first entry that exists. if
  // none exist, return NULL (safe because we check again for existence later)
  QString find_image(QStringList p_list);

protected:
  AOApplication *ao_app;
  QVector<QPixmap> movie_frames;
  QVector<int> movie_delays;

  QTimer *preanim_timer;
  QTimer *shfx_timer;
  QTimer *ticker;
  QString last_path;
  QImageReader m_reader;

  QElapsedTimer actual_time;

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
  int last_max_frames = 0;

  int speed = 100;

  bool m_flipped = false;

  int duration = 0;

  // Start playback of the movie (if animated).
  void play();

  // Freeze the movie at the current frame.
  void freeze();

  // Retreive a pixmap adjused for mirroring/aspect ratio shenanigans from a
  // provided QImage
  QPixmap get_pixmap(QImage image);

  // Set the movie's frame to provided pixmap
  void set_frame(QPixmap f_pixmap);
  // Center the QLabel in the viewport based on the dimensions of f_pixmap
  void center_pixmap(QPixmap f_pixmap);

private:
  // Populates the frame and delay vectors.
  void populate_vectors();

  // used in populate_vectors
  void load_next_frame();
  std::atomic_bool exit_loop { false }; //awful solution but i'm not fucking using QThread
  QFuture<void> frame_loader;
  QMutex mutex;
  QWaitCondition frameAdded;


signals:
  void done();

protected slots:
  virtual void preanim_done();
  void shfx_timer_done();
  virtual void movie_ticker();
};

class BackgroundLayer : public AOLayer {
  Q_OBJECT
public:
  BackgroundLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename);
};

class CharLayer : public AOLayer {
  Q_OBJECT
public:
  CharLayer(QWidget *p_parent, AOApplication *p_ao_app);
  QString current_emote = ""; // name of the emote we're using
  bool is_preanim;     // equivalent to the old play_once, if true we don't want
                       // to loop this
  QString prefix = ""; // prefix, left blank if it's a preanim

  void load_image(QString p_filename, QString p_charname, int p_duration, bool p_is_preanim);
  void play(); // overloaded so we can play effects

  // networked frame fx string
  QStringList network_strings;

private:
  QString last_char;        // name of the last character we used
  QString last_emote;       // name of the last animation we used
  QString last_prefix;      // prefix of the last animation we played
  bool was_preanim = false; // whether is_preanim was true last time

  // Effects such as sfx, screenshakes and realization flashes are stored in
  // here. QString entry format: "sfx^[sfx_name]", "shake", "flash". The program
  // uses the QVector index as reference.
  QVector<QVector<QString>> movie_effects;

  // used for effect loading
  QString m_char = "";
  QString m_emote = "";

  // overloaded for effects reasons
  void start_playback(QString p_image);

  // Initialize the frame-specific effects from the char.ini
  void load_effects();

  // Initialize the frame-specific effects from the provided network_strings,
  // this is only initialized if network_strings has size more than 0.
  void load_network_effects();

  // Play a frame-specific effect, if there's any defined for that specific
  // frame.
  void play_frame_effect(int p_frame);

private slots:
  void preanim_done() override; // overridden so we don't accidentally cull characters
  void movie_ticker() override; // overridden so we can play effects

signals:
  void shake();
  void flash();
  void play_sfx(QString sfx);
};

class SplashLayer : public AOLayer {
  Q_OBJECT
public:
  SplashLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, QString p_charname, QString p_miscname);
};

class EffectLayer : public AOLayer {
  Q_OBJECT
public:
  EffectLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, bool p_looping);
};

class InterfaceLayer : public AOLayer {
  Q_OBJECT
public:
  InterfaceLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, QString p_miscname);
};

class StickerLayer : public AOLayer {
  Q_OBJECT
public:
  StickerLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_charname);
};

#endif // AOLAYER_H
