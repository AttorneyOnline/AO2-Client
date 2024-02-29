#include "aolayer.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "misc_functions.h"
#include "options.h"

static QThreadPool *thread_pool;

AOLayer::AOLayer(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  // used for culling images when their max_duration is exceeded
  shfx_timer = new QTimer(this);
  shfx_timer->setTimerType(Qt::PreciseTimer);
  shfx_timer->setSingleShot(true);
  connect(shfx_timer, &QTimer::timeout, this, &AOLayer::shfx_timer_done);

  ticker = new QTimer(this);
  ticker->setTimerType(Qt::PreciseTimer);
  ticker->setSingleShot(false);
  connect(ticker, &QTimer::timeout, this, &AOLayer::movie_ticker);

  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);
  connect(preanim_timer, &QTimer::timeout, this, &AOLayer::preanim_done);

  if (!thread_pool) {
    thread_pool = new QThreadPool(p_ao_app);
    thread_pool->setMaxThreadCount(8);
  }
}

BackgroundLayer::BackgroundLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}
CharLayer::CharLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}
EffectLayer::EffectLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}
SplashLayer::SplashLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}
InterfaceLayer::InterfaceLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}
StickerLayer::StickerLayer(QWidget *p_parent, AOApplication *p_ao_app)
    : AOLayer(p_parent, p_ao_app)
{
}

QString AOLayer::find_image(QStringList p_list)
{
  QString image_path;
  for (const QString &path : p_list) {
#ifdef DEBUG_MOVIE
    qDebug() << "checking path " << path;
#endif
    if (file_exists(path)) {
      image_path = path;
#ifdef DEBUG_MOVIE
      qDebug() << "found    path " << path;
#endif
      break;
    }
  }
  return image_path;
}

QPixmap AOLayer::get_pixmap(QImage image)
{
  QPixmap f_pixmap;
  if (m_flipped)
    f_pixmap = QPixmap::fromImage(image.mirrored(true, false));
  else
    f_pixmap = QPixmap::fromImage(image);
  //    auto aspect_ratio = Qt::KeepAspectRatio;
  if (!f_pixmap.isNull()) {
    if (f_pixmap.height() > f_h) // We are downscaling, use anti-aliasing.
      transform_mode = Qt::SmoothTransformation;
    if (stretch)
      f_pixmap = f_pixmap.scaled(f_w, f_h);
    else
      f_pixmap = f_pixmap.scaledToHeight(f_h, transform_mode);
    this->resize(f_pixmap.size());
  }
  return f_pixmap;
}

void AOLayer::set_frame(QPixmap f_pixmap)
{
  this->setPixmap(f_pixmap);
  this->center_pixmap(f_pixmap);
}

void AOLayer::center_pixmap(QPixmap f_pixmap) {
  QLabel::move(
      x + (f_w - f_pixmap.width()) / 2,
      y + (f_h - f_pixmap.height())); // Always center horizontally, always put
                                      // at the bottom vertically
  if (masked)
      this->setMask(
          QRegion((f_pixmap.width() - f_w) / 2, (f_pixmap.height() - f_h) / 2, f_w,
                  f_h)); // make sure we don't escape the area we've been given
}

void AOLayer::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  f_w = w;
  f_h = h;
  this->resize(f_size);
}

int AOLayer::get_frame_delay(int delay)
{
  return static_cast<int>(double(delay) * double(speed / 100));
}

void AOLayer::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}

void AOLayer::move_and_center(int ax, int ay)
{
  x = ax;
  y = ay;
  if (movie_frames.isEmpty()) // safeguard
    QLabel::move(x,y);
  else
    center_pixmap(movie_frames[0]); // just use the first frame since dimensions are all that matter
}

void BackgroundLayer::load_image(QString p_filename)
{
  play_once = false;
  cull_image = false;
  VPath design_path = ao_app->get_background_path("design.ini");
  transform_mode =
      ao_app->get_scaling(ao_app->read_design_ini("scaling", design_path));
  stretch = ao_app->read_design_ini("stretch", design_path).startsWith("true");
#ifdef DEBUG_MOVIE
  qDebug() << "[BackgroundLayer] BG loaded: " << p_filename;
#endif
  QString final_path = ao_app->get_image_suffix(ao_app->get_background_path(p_filename));

  if (final_path == last_path) {
    // Don't restart background if background is unchanged
    return;
  }

  start_playback(final_path);
  play();
}

void CharLayer::load_image(QString p_filename, QString p_charname,
                           int p_duration, bool p_is_preanim)
{
  duration = p_duration;
  cull_image = false;
  force_continuous = false;
  transform_mode = ao_app->get_scaling(
      ao_app->get_emote_property(p_charname, p_filename, "scaling"));
  stretch = ao_app->get_emote_property(p_charname, p_filename, "stretch")
                .startsWith("true");
  if ((p_charname == last_char) &&
      ((p_filename == last_emote) ||
       (p_filename.mid(3, -1) == last_emote.mid(3, -1))) &&
      (!is_preanim) && (!was_preanim)) {
    continuous = true;
    force_continuous = true;
  }
  else {
    continuous = false;
    force_continuous = true;
  }
  prefix = "";
  current_emote = p_filename;
  was_preanim = is_preanim;
  m_char = p_charname;
  m_emote = current_emote;
  last_char = p_charname;
  last_emote = current_emote;
  last_prefix = prefix;
  is_preanim = p_is_preanim;
  if ((p_filename.left(3) == "(a)") || (p_filename.left(3) == "(b)")) { // if we are playing an idle or talking animation
    prefix = p_filename.left(3); // separate the prefix from the emote name
    current_emote = p_filename.mid(3, -1);
  }
  else if ((duration > 0) || (p_filename.left(3) == "(c)")) { // else if we are playing a preanim or postanim
    if (p_filename.left(3) == "(c)") { // if we are playing a postanim
      prefix = "(c)"; // separate the prefix from the emote name
      current_emote = p_filename.mid(3, -1);
    }
    // pre/postanim specific flags
    is_preanim = true;
    play_once = true;
    preanim_timer->start(duration);
  }
#ifdef DEBUG_MOVIE
  qDebug() << "[CharLayer] anim loaded: prefix " << prefix << " filename "
           << current_emote << " from character: " << p_charname
           << " continuous: " << continuous;
#endif
  QVector<VPath> pathlist { // cursed character path resolution vector
      ao_app->get_character_path(
          p_charname, prefix + current_emote), // Default path
      ao_app->get_character_path(
          p_charname,
          prefix + "/" + current_emote), // Path check if it's categorized
                                          // into a folder
      ao_app->get_character_path(
          p_charname,
          current_emote), // Just use the non-prefixed image, animated or not
      VPath(current_emote), // The path by itself after the above fail
      ao_app->get_theme_path("placeholder"), // Theme placeholder path
      ao_app->get_theme_path(
          "placeholder", ao_app->default_theme)}; // Default theme placeholder path
  start_playback(ao_app->get_image_path(pathlist));
  play();
}

void SplashLayer::load_image(QString p_filename, QString p_charname,
                                   QString p_miscname)
{
  transform_mode = ao_app->get_misc_scaling(p_miscname);
  QString final_image = ao_app->get_image(p_filename, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, p_miscname, p_charname, "placeholder");
  start_playback(final_image);
  play();
}

void EffectLayer::load_image(QString p_filename, bool p_looping)
{
  if (p_looping)
    play_once = false;
  else
    play_once = true;
  continuous = false;
  force_continuous = true;
  cull_image = false;

  start_playback(p_filename); // path resolution is handled by the caller for EffectLayer objects
  play();
}

void InterfaceLayer::load_image(QString p_filename, QString p_miscname)
{
  last_path = "";
  stretch = true;
  QString final_image = ao_app->get_image(p_filename, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, p_miscname);
  start_playback(final_image);
  play();
}

void StickerLayer::load_image(QString p_charname)
{
  QString p_miscname;
  if (Options::getInstance().customChatboxEnabled())
    p_miscname = ao_app->get_chat(p_charname);
  transform_mode = ao_app->get_misc_scaling(p_miscname);
  QString final_image = ao_app->get_image("sticker/" + p_charname, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, p_miscname);
  start_playback(final_image);
  play();
}

void CharLayer::start_playback(QString p_image)
{
  movie_effects.clear();
  AOLayer::start_playback(p_image);
  if (network_strings.size() > 0) // our FX overwritten by networked ones
    load_network_effects();
  else // Use default ini FX
    load_effects();
  play();
}

void AOLayer::start_playback(QString p_image)
{
  if (p_image == "") {// image wasn't found by the path resolution function
    this->kill();
    return;
  }

  if (frame_loader.isRunning())
    exit_loop = true; // tell the loader to stop, we have a new image to load

  QMutexLocker locker(&mutex);
  this->show();

  if (!Options::getInstance().continuousPlaybackEnabled()) {
    continuous = false;
    force_continuous = true;
  }

  if (((last_path == p_image) && (!force_continuous)) || p_image == "")
    return;

#ifdef DEBUG_MOVIE
  actual_time.restart();
#endif
  this->clear();
  this->freeze();
  movie_frames.clear();
  movie_delays.clear();
  QString scaling_override =
      ao_app->read_design_ini("scaling", p_image + ".ini");
  if (scaling_override != "")
    transform_mode = ao_app->get_scaling(scaling_override);
  QString stretch_override =
      ao_app->read_design_ini("stretch", p_image + ".ini");
  if (stretch_override != "")
    stretch = stretch_override.startsWith("true");

#ifdef DEBUG_MOVIE
  qDebug() << "[AOLayer::start_playback] Stretch:" << stretch << "Filename:" << p_image;
#endif
  m_reader.setFileName(p_image);
  last_max_frames = max_frames;
  max_frames = m_reader.imageCount();
  if (m_reader.loopCount() == 0 && max_frames > 1)
    play_once = true;
  if (!continuous
          || ((continuous) && (max_frames != last_max_frames))
          || max_frames == 0
          || frame >= max_frames) {
    frame = 0;
    continuous = false;
  }
  #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  frame_loader = QtConcurrent::run(thread_pool, this, &AOLayer::populate_vectors);
  #else
  frame_loader = QtConcurrent::run(thread_pool, &AOLayer::populate_vectors, this);
  #endif
  last_path = p_image;
  while (movie_frames.size() <= frame) // if we haven't loaded the frame we need yet
    frameAdded.wait(&mutex); // wait for the frame loader to add another frame, then check again
  this->set_frame(movie_frames[frame]);

  if (max_frames <= 1) {
    duration = static_duration;
#ifdef DEBUG_MOVIE
    qDebug() << "[AOLayer::start_playback] max_frames is <= 1, using static duration";
#endif
  }
  if (duration > 0 && cull_image == true)
    shfx_timer->start(duration);
#ifdef DEBUG_MOVIE
  qDebug() << "[AOLayer::start_playback] Max frames:" << max_frames << "Setting image to " << p_image
           << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif
}

void CharLayer::play()
{
  if (max_frames <= 1) {
    if (play_once) {
      preanim_timer->start(qMax(0, duration));
    }
    return;
  }
  play_frame_effect(frame);
  AOLayer::play();
}

void AOLayer::play()
{
  if (max_frames <= 1) {
    if (play_once) {
      if (duration > 0)
        ticker->start(duration);
      else
        preanim_done();
    }
    else
      this->freeze();
  }
  else {
    while (movie_delays.size() <= frame) {
        frameAdded.wait(&mutex);
    }
    ticker->start(this->get_frame_delay(movie_delays[frame]));
  }
}

void AOLayer::set_play_once(bool p_play_once) { play_once = p_play_once; }
void AOLayer::set_cull_image(bool p_cull_image) { cull_image = p_cull_image; }
void AOLayer::set_static_duration(int p_static_duration)
{
  static_duration = p_static_duration;
}
void AOLayer::set_max_duration(int p_max_duration)
{
  max_duration = p_max_duration;
}

void CharLayer::load_effects()
{
  movie_effects.clear();
  if (max_frames <= 1)
    return;
  movie_effects.resize(max_frames);
  for (int e_frame = 0; e_frame < max_frames; ++e_frame) {
    QString effect = ao_app->get_screenshake_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("shake");
    }

    effect = ao_app->get_flash_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("flash");
    }

    effect = ao_app->get_sfx_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("sfx^" + effect);
    }
  }
}

void CharLayer::load_network_effects()
{
  movie_effects.clear();
  if (max_frames <= 1)
    return;
  movie_effects.resize(max_frames);
  // Order is important!!!
  QStringList effects_list = {"shake", "flash", "sfx^"};

  // Determines which list is smaller - effects_list or network_strings - and
  // uses it as basis for the loop. This way, incomplete network_strings would
  // still be parsed, and excess/unaccounted for networked information is
  // omitted.
  int effects_size = qMin(effects_list.size(), network_strings.size());

  for (int i = 0; i < effects_size; ++i) {
    QString netstring = network_strings.at(i);
    QStringList emote_splits = netstring.split("^");
    for (const QString &emote : emote_splits) {
      QStringList parsed = emote.split("|");
      if (parsed.size() <= 0 || parsed.at(0) != m_emote)
        continue;
      foreach (QString frame_data, parsed) {
        QStringList frame_split = frame_data.split("=");
        if (frame_split.size() <=
            1) // We might still be hanging at the emote itself (entry 0).
          continue;
        int f_frame = frame_split.at(0).toInt();
        if (f_frame >= max_frames || f_frame < 0) {
          qWarning() << "out of bounds" << effects_list[i] << "frame"
                   << f_frame << "out of" << max_frames << "for" << m_emote;
          continue;
        }
        QString f_data = frame_split.at(1);
        if (f_data != "") {
          QString effect = effects_list[i];
          if (effect == "sfx^") // Currently the only frame result that feeds us
                                // data, let's yank it in.
            effect += f_data;
#ifdef DEBUG_MOVIE
          qDebug() << "[CharLayer::load_network_effects]" << effect << f_data << "frame" << f_frame << "for"
                   << m_emote;
#endif
          movie_effects[f_frame].append(effect);
        }
      }
    }
  }
}

void CharLayer::play_frame_effect(int p_frame)
{
  if (p_frame >= movie_effects.size()) {
    qWarning() << "Attempted to play a frame effect bigger than the size of movie_effects";
    return;
  }
  if (p_frame < max_frames) {
    foreach (QString effect, movie_effects[p_frame]) {
      if (effect == "shake") {
        emit shake();
#ifdef DEBUG_MOVIE
        qDebug() << "[CharLayer::play_frame_effect] Attempting to play shake on frame" << frame;
#endif
      }

      if (effect == "flash") {
        emit flash();
#ifdef DEBUG_MOVIE
        qDebug() << "[CharLayer::play_frame_effect] Attempting to play flash on frame" << frame;
#endif
      }

      if (effect.startsWith("sfx^")) {
        QString sfx = effect.section("^", 1);
        emit play_sfx(sfx);
#ifdef DEBUG_MOVIE
        qDebug() << "[CharLayer::play_frame_effect] Attempting to play sfx" << sfx << "on frame" << frame;
#endif
      }
    }
  }
}

void AOLayer::stop()
{
  // for all intents and purposes, stopping is the same as hiding. at no point
  // do we want a frozen gif to display
  this->freeze();
  this->hide();
  shfx_timer->stop();
}

void AOLayer::freeze()
{
  // aT nO pOiNt Do We WaNt A fRoZeN gIf To DiSpLaY
  ticker->stop();
  preanim_timer->stop();
}

void AOLayer::kill()
{
  // used for when we want to ensure a file is loaded anew
  this->stop();
  this->clear();
  movie_frames.clear();
  movie_delays.clear();
  last_max_frames = max_frames;
  max_frames = 0;
  last_path = "";
}

void CharLayer::movie_ticker()
{
  AOLayer::movie_ticker();
  play_frame_effect(frame);
}

void AOLayer::movie_ticker()
{
  ++frame;
  if (frame >= max_frames) {
    if (play_once) {
      if (cull_image)
        this->stop();
      else
        this->freeze();
      preanim_done();
      return;
    }
    else
      frame = 0;
  }
  {
    QMutexLocker locker(&mutex);
    while (frame >= movie_frames.size() && frame < max_frames) // oops! our frame isn't ready yet
      frameAdded.wait(&mutex); // wait for a new frame to be added, then check again
  }
#ifdef DEBUG_MOVIE
  qDebug() << "[AOLayer::movie_ticker] Frame:" << frame << "Delay:" << movie_delays[frame]
           << "Actual time taken from last frame:" << actual_time.restart();
#endif
  this->set_frame(movie_frames[frame]);
  ticker->setInterval(this->get_frame_delay(movie_delays[frame]));
}

void AOLayer::populate_vectors() {
#ifdef DEBUG_MOVIE
  qDebug() << "[AOLayer::populate_vectors] Started thread";
#endif
  while (!exit_loop && movie_frames.size() < max_frames) {
    load_next_frame();
#ifdef DEBUG_MOVIE
    qDebug() << "[AOLayer::populate_vectors] Loaded frame" << movie_frames.size();
#endif
  }
#ifdef DEBUG_MOVIE
  if (exit_loop)
    qDebug() << "[AOLayer::populate_vectors] Exit requested";
#endif
  exit_loop = false;
}

void AOLayer::load_next_frame() {
  {
    QMutexLocker locker(&mutex);
    movie_frames.append(this->get_pixmap(m_reader.read()));
    movie_delays.append(m_reader.nextImageDelay());
  }
  frameAdded.wakeAll();
}

void CharLayer::preanim_done()
{
  if (is_preanim)
    AOLayer::preanim_done();
  else
    return;
}

void AOLayer::preanim_done()
{
  ticker->stop();
  preanim_timer->stop();
  emit done();
}

void AOLayer::shfx_timer_done()
{
  this->stop();
#ifdef DEBUG_MOVIE
  qDebug() << "shfx timer signaled done";
#endif
  // signal connected to courtroom object, let it figure out what to do
  emit done();
}
