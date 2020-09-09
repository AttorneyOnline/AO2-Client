#include "aolayer.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "misc_functions.h"

AOLayer::AOLayer(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  shfx_timer = new QTimer(this);
  shfx_timer->setTimerType(Qt::PreciseTimer);
  shfx_timer->setSingleShot(true);
  connect(shfx_timer, SIGNAL(timeout()), this, SLOT(shfx_timer_done()));

  ticker = new QTimer(this);
  ticker->setTimerType(Qt::PreciseTimer);
  ticker->setSingleShot(false);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));

  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOLayer::load_image(LayerProperties p_props)
{
#ifdef DEBUG_MOVIE
  actual_time.restart();
#endif
  QList<QString> pathlist;
  is_effect = false;
  int duration = p_props.max_duration;
  switch (p_props.type) {
  case LayerType::background: {
    pathlist = {ao_app->get_image_suffix(ao_app->get_background_path(
                    p_props.filename)), // check current bg
                ao_app->get_image_suffix(ao_app->get_default_background_path(
                    p_props.filename))}; // then check "default"
    break;
  }
  case LayerType::foreground: {
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_misc_path(
            p_props.miscname, p_props.filename)), // check our misc folder first
        ao_app->get_image_suffix(ao_app->get_theme_path(
            p_props.filename)), // then check the user's theme
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            p_props.filename))}; // and finally check the default theme
    break;
  }
  case LayerType::character: {
    QString prefix = "";
    QString emote = p_props.filename;
    if ((p_props.filename.left(3) == "(a)") ||
        (p_props.filename.left(3) == "(b)")) {
      prefix = p_props.filename.left(3);
      emote = p_props.filename.mid(3, -1);
    }
    qDebug() << "[AOLayer] emote loaded: prefix " << prefix << " filename "
             << emote;
    pathlist = {ao_app->get_image_suffix(ao_app->get_character_path(
                    p_props.charname, prefix + emote)), // Default path
                ao_app->get_image_suffix(ao_app->get_character_path(
                    p_props.charname,
                    prefix + "/" + emote)), // Path check if it's categorized
                                            // into a folder
                ao_app->get_image_suffix(ao_app->get_character_path(
                    p_props.charname,
                    emote)), // Just use the non-prefixed image, animated or not
                ao_app->get_image_suffix(ao_app->get_theme_path(
                    "placeholder")), // Theme placeholder path
                ao_app->get_image_suffix(ao_app->get_default_theme_path(
                    "placeholder"))}; // Default theme placeholder path
    m_char = p_props.charname;
    m_emote = p_props.filename;
    if (duration > 0)
      preanim_timer->start(duration * tick_ms);
    break;
  }
  case LayerType::interjection: {
    play_once = true;
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_props.charname, p_props.filename)), // Character folder
        ao_app->get_image_suffix(ao_app->get_misc_path(
            p_props.miscname, p_props.filename)), // Misc path
        ao_app->get_image_suffix(
            ao_app->get_theme_path(p_props.filename)), // Theme path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            p_props.filename)), // Default theme path
        ao_app->get_image_suffix(
            ao_app->get_theme_path("placeholder")), // Placeholder path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "placeholder")), // Default placeholder path
    };
    break;
  }
  case LayerType::effect: {
    is_effect = true;
    pathlist = {p_props.filename}; // this is handled in text_file_functions
    break;
  }
  case LayerType::ui: {
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_misc_path(
            p_props.miscname, p_props.filename)), // check our misc folder first
        ao_app->get_image_suffix(ao_app->get_theme_path(
            p_props.filename)), // then check the user's theme
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            p_props.filename))}; // and finally check the default theme
    break;
  }
  }
  QString image_path;
  for (QString path : pathlist) {
#ifdef DEBUG_MOVIE
    qDebug() << "checking path " << path;
#endif
    if (file_exists(path)) {
      image_path = path;
#ifdef DEBUG_MOVIE
      qDebug() << "found path " << path;
#endif
      break;
    }
  }

  this->clear();
  ticker->stop();
  preanim_timer->stop();
  shfx_timer->stop();
  movie_frames.clear();
  movie_delays.clear();
  movie_effects.clear();

  if (!file_exists(image_path))
    return;

  m_reader->setFileName(image_path);
  if (m_reader->loopCount() == 0)
    play_once = true;
  QPixmap f_pixmap = this->get_pixmap(m_reader->read());
  int f_delay = m_reader->nextImageDelay();

  frame = 0;
  max_frames = m_reader->imageCount();

  this->set_frame(f_pixmap);
  this->show();
  if (max_frames > 1) {
    movie_frames.append(f_pixmap);
    movie_delays.append(f_delay);
  }
  else if (max_frames <= 1) {
    duration = p_props.static_duration;
    play_once = false; // we don't want to call movie_ticker on a static image
#ifdef DEBUG_MOVIE
    qDebug() << "max_frames is <= 1, using static duration";
#endif
  }
  if (network_strings.size() > 0) // our FX overwritten by networked ones
    this->load_network_effects();
  else // Use default ini FX
    this->load_effects();

  if (duration > 0)
    shfx_timer->start(duration);
  play();
#ifdef DEBUG_MOVIE
  qDebug() << max_frames << "Setting image to " << image_path
           << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif
}

void AOLayer::set_play_once(bool p_play_once) { play_once = p_play_once; }

void AOLayer::load_effects()
{
  movie_effects.clear();
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

void AOLayer::load_network_effects()
{
  movie_effects.clear();
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
    foreach (QString emote, emote_splits) {
      QStringList parsed = emote.split("|");
      if (parsed.size() <= 0 || parsed.at(0) != m_emote)
        continue;
      foreach (QString frame_data, parsed) {
        QStringList frame_split = frame_data.split("=");
        if (frame_split.size() <=
            1) // We might still be hanging at the emote itself (entry 0).
          continue;
        int f_frame = frame_split.at(0).toInt();
        if (f_frame >= max_frames) {
          qDebug() << "Warning: out of bounds" << effects_list[i] << "frame"
                   << f_frame << "out of" << max_frames << "for" << m_char
                   << m_emote;
          continue;
        }
        QString f_data = frame_split.at(1);
        if (f_data != "") {
          QString effect = effects_list[i];
          if (effect == "sfx^") // Currently the only frame result that feeds us
                                // data, let's yank it in.
            effect += f_data;
          qDebug() << effect << f_data << "frame" << f_frame << "for" << m_char
                   << m_emote;
          movie_effects[f_frame].append(effect);
        }
      }
    }
  }
}

void AOLayer::play()
{
  play_frame_effect(frame);
  if (max_frames <= 1) {
    if (play_once)
      ticker->start(tick_ms);
    else
      this->freeze();
  }
  else
    ticker->start(this->get_frame_delay(movie_delays[frame]));
}

void AOLayer::play_frame_effect(int frame)
{
  if (frame < max_frames) {
    foreach (QString effect, movie_effects[frame]) {
      if (effect == "shake") {
        shake();
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play shake on frame" << frame;
#endif
      }

      if (effect == "flash") {
        flash();
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play flash on frame" << frame;
#endif
      }

      if (effect.startsWith("sfx^")) {
        QString sfx = effect.section("^", 1);
        play_sfx(sfx);
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play sfx" << sfx << "on frame" << frame;
#endif
      }
    }
  }
}

void AOLayer::stop()
{
  // for all intents and purposes, stopping is the same as hiding. at no point
  // do we want a frozen gif to display
  ticker->stop();
  preanim_timer->stop();
  shfx_timer->stop();
  this->hide();
}

void AOLayer::freeze()
{
  // aT nO pOiNt Do We WaNt A fRoZeN gIf To DiSpLaY
  ticker->stop();
  preanim_timer->stop();
  shfx_timer->stop();
}

QPixmap AOLayer::get_pixmap(QImage image)
{
  QPixmap f_pixmap;
  if (m_flipped)
    f_pixmap = QPixmap::fromImage(image.mirrored(true, false));
  else
    f_pixmap = QPixmap::fromImage(image);
  //    auto aspect_ratio = Qt::KeepAspectRatio;
  auto transform_mode = Qt::FastTransformation;
  if (f_pixmap.height() > f_h) // We are downscaling, use anti-aliasing.
    transform_mode = Qt::SmoothTransformation;
  if (is_effect)
    f_pixmap = f_pixmap.scaled(f_w, f_h, Qt::IgnoreAspectRatio, transform_mode);
  else
    f_pixmap = f_pixmap.scaledToHeight(f_h, transform_mode);
  this->resize(f_pixmap.size());

  return f_pixmap;
}

void AOLayer::set_frame(QPixmap f_pixmap)
{
  this->setPixmap(f_pixmap);
  QLabel::move(
      x + (f_w - f_pixmap.width()) / 2,
      y + (f_h - f_pixmap.height())); // Always center horizontally, always put
                                      // at the bottom vertically
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

void AOLayer::movie_ticker()
{
  ++frame;
  if ((frame >= max_frames) && (max_frames > 1)) {
    if (play_once) {
      this->stop();
      preanim_done();
      return;
    }
    else
      frame = 0;
  }
  //  qint64 difference = elapsed - movie_delays[frame];
  if (frame >= movie_frames.size()) {
    m_reader->jumpToImage(frame);
    movie_frames.resize(frame + 1);
    movie_frames[frame] = this->get_pixmap(m_reader->read());
    movie_delays.resize(frame + 1);
    movie_delays[frame] = m_reader->nextImageDelay();
  }

#ifdef DEBUG_MOVIE
  qDebug() << frame << movie_delays[frame]
           << "actual time taken from last frame:" << actual_time.restart();
#endif

  this->set_frame(movie_frames[frame]);
  play_frame_effect(frame);
  ticker->setInterval(this->get_frame_delay(movie_delays[frame]));
}

void AOLayer::preanim_done()
{
  ticker->stop();
  preanim_timer->stop();
  done();
}

void AOLayer::shfx_timer_done()
{
  this->stop();
#ifdef DEBUG_MOVIE
  qDebug() << "shfx timer signaled done";
#endif
  // signal connected to courtroom object, let it figure out what to do
  done();
}
