#include "aolayer.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "misc_functions.h"

AOLayer::AOLayer(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  // used for culling images when their max_duration is exceeded
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

QString AOLayer::find_image(QList<QString> p_list)
{
  QString image_path;
  for (QString path : p_list) {
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
  if (f_pixmap.height() > f_h) // We are downscaling, use anti-aliasing.
    transform_mode = Qt::SmoothTransformation;
  if (stretch)
    f_pixmap = f_pixmap.scaled(f_w, f_h);
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

void BackgroundLayer::load_image(QString p_filename)
{
  play_once = false;
  cull_image = false;
  QString design_path = ao_app->get_background_path("design.ini");
  transform_mode =
      ao_app->get_scaling(ao_app->read_design_ini("scaling", design_path));
  stretch = ao_app->read_design_ini("stretch", design_path).startsWith("true");
  qDebug() << "[BackgroundLayer] BG loaded: " << p_filename;
  start_playback(ao_app->get_image_suffix(ao_app->get_background_path(p_filename)));
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
                .startsWith(true);
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
  if ((p_filename.left(3) == "(a)") || (p_filename.left(3) == "(b)")) {
    prefix = p_filename.left(3);
    current_emote = p_filename.mid(3, -1);
  }
  else if ((duration > 0) || (p_filename.left(3) == "(c)")) {
    if (p_filename.left(3) == "(c)") {
      prefix = "(c)";
      current_emote = p_filename.mid(3, -1);
    }
    is_preanim = true;
    play_once = true;
    preanim_timer->start(duration * tick_ms);
  }
  qDebug() << "[CharLayer] anim loaded: prefix " << prefix << " filename "
           << current_emote << " from character: " << p_charname
           << " continuous: " << continuous;
  QList<QString> pathlist = {
      ao_app->get_image_suffix(ao_app->get_character_path(
          p_charname, prefix + current_emote)), // Default path
      ao_app->get_image_suffix(ao_app->get_character_path(
          p_charname,
          prefix + "/" + current_emote)), // Path check if it's categorized
                                          // into a folder
      ao_app->get_image_suffix(ao_app->get_character_path(
          p_charname,
          current_emote)), // Just use the non-prefixed image, animated or not
      ao_app->get_image_suffix(
          ao_app->get_theme_path("placeholder")), // Theme placeholder path
      ao_app->get_image_suffix(ao_app->get_default_theme_path(
          "placeholder"))}; // Default theme placeholder path
  this->start_playback(find_image(pathlist));
}

void SplashLayer::load_image(QString p_filename, QString p_charname,
                                   QString p_miscname)
{
  transform_mode = ao_app->get_misc_scaling(p_miscname);
  QList<QString> pathlist = {
      ao_app->get_image_suffix(ao_app->get_character_path(
          p_charname, p_filename)), // Character folder
      ao_app->get_image_suffix(ao_app->get_theme_path(
          "misc/" + p_miscname + "/" + p_filename)), // Theme misc path
      ao_app->get_image_suffix(
          ao_app->get_misc_path(p_miscname, p_filename)), // Misc path
      ao_app->get_image_suffix(
          ao_app->get_theme_path(p_filename)), // Theme path
      ao_app->get_image_suffix(
          ao_app->get_default_theme_path(p_filename)), // Default theme path
      ao_app->get_image_suffix(
          ao_app->get_theme_path("placeholder")), // Placeholder path
      ao_app->get_image_suffix(ao_app->get_default_theme_path(
          "placeholder")), // Default placeholder path
  };
  QString final_image = find_image(pathlist);
  if (final_image == ao_app->get_theme_path("custom.png") ||
      final_image == ao_app->get_default_theme_path("custom.png") ||
      final_image == ao_app->get_theme_path("witnesstestimony.png") ||
      final_image == ao_app->get_default_theme_path("witnesstestimony.png") ||
      final_image == ao_app->get_theme_path("crossexamination.png") ||
      final_image == ao_app->get_default_theme_path("crossexamination.png")) 
    // stupid exceptions because themes are stupid
    final_image = find_image(
        {ao_app->get_image_suffix(ao_app->get_theme_path("placeholder")),
         ao_app->get_image_suffix(ao_app->get_default_theme_path("placeholder"))});
  start_playback(final_image);
}

void EffectLayer::load_image(QString p_filename, bool p_looping)
{
  if (p_looping)
    play_once = false;
  else
    play_once = true;
  continuous = false;
  force_continuous = true;
  start_playback(p_filename); // handled in its own file before we see it
}

void InterfaceLayer::load_image(QString p_filename, QString p_miscname)
{
  stretch = true;
  QList<QString> pathlist = {
      ao_app->get_image_suffix(ao_app->get_theme_path(
          "misc/" + p_miscname + "/" +
          p_filename)), // first check our theme's misc directory
      ao_app->get_image_suffix(ao_app->get_misc_path(
          p_miscname, p_filename)), // then check our global misc folder
      ao_app->get_image_suffix(ao_app->get_theme_path(
          p_filename)), // then check the user's theme for a default image
      ao_app->get_image_suffix(ao_app->get_default_theme_path(
          p_filename))}; // and finally check the default theme
  start_playback(find_image(pathlist));
}

void StickerLayer::load_image(QString p_charname)
{
  QString miscname = ao_app->get_char_shouts(p_charname);
  transform_mode = ao_app->get_misc_scaling(miscname);
  QList<QString> pathlist = {
      ao_app->get_image_suffix(ao_app->get_base_path() + "misc/" +
                                miscname + "/sticker/" + p_charname), // Misc path
      ao_app->get_image_suffix(ao_app->get_custom_theme_path(miscname, "sticker/" + p_charname)), // Custom theme path
      ao_app->get_image_suffix(ao_app->get_theme_path("sticker/" + p_charname)), // Theme path
      ao_app->get_image_suffix(
          ao_app->get_default_theme_path("sticker/" + p_charname)), // Default theme path
      ao_app->get_image_suffix(
          ao_app->get_character_path(p_charname, "sticker")), // Character folder
      ao_app->get_image_suffix(
          ao_app->get_character_path(p_charname, "showname")), // Scuffed DRO way
  };
  start_playback(find_image(pathlist));
}

void CharLayer::start_playback(QString p_image)
{
  movie_effects.clear();
  AOLayer::start_playback(p_image);
  if (network_strings.size() > 0) // our FX overwritten by networked ones
    load_network_effects();
  else // Use default ini FX
    load_effects();
}

void AOLayer::start_playback(QString p_image)
{
#ifdef DEBUG_MOVIE
  actual_time.restart();
#endif
  this->clear();
  freeze();
  movie_frames.clear();
  movie_delays.clear();

  if (!file_exists(p_image))
    return;

  QString scaling_override =
      ao_app->read_design_ini("scaling", p_image + ".ini");
  if (scaling_override != "")
    transform_mode = ao_app->get_scaling(scaling_override);
  QString stretch_override =
      ao_app->read_design_ini("stretch", p_image + ".ini");
  if (stretch_override != "")
    stretch = stretch_override.startsWith("true");

  qDebug() << "stretch:" << stretch << "filename:" << p_image;
  m_reader.setFileName(p_image);
  if (m_reader.loopCount() == 0)
    play_once = true;
  if ((last_path == p_image) && (!force_continuous))
    continuous = true;
  else if ((last_path != p_image) && !force_continuous)
    continuous = false;
  if (!continuous)
    frame = 0;
  force_continuous = false;
  last_max_frames = max_frames;
  max_frames = m_reader.imageCount();
  if (((continuous) && (max_frames != last_max_frames)) || max_frames == 0) {
    frame = 0;
    continuous = false;
  }
  // CANTFIX: this causes a slight hitch
  // The correct way of doing this would be to use QImageReader::jumpToImage()
  // and populate missing data in the movie ticker when it's needed. This is
  // unforunately completely impossible, because QImageReader::jumpToImage() is
  // not implemented in any image format AO2 is equipped to use. Instead, the
  // default behavior is used - that is, absolutely nothing.
  if (continuous) {
    for (int i = frame; i--;) {
      if (i <= -1)
        break;
      QPixmap l_pixmap = this->get_pixmap(m_reader.read());
      int l_delay = m_reader.nextImageDelay();
      movie_frames.append(l_pixmap);
      movie_delays.append(l_delay);
    }
  }
  QPixmap f_pixmap = this->get_pixmap(m_reader.read());
  int f_delay = m_reader.nextImageDelay();

  this->set_frame(f_pixmap);
  this->show();
  if (max_frames > 1) {
    movie_frames.append(f_pixmap);
    movie_delays.append(f_delay);
  }
  else if (max_frames <= 1) {
    duration = static_duration;
    play_once = false;
#ifdef DEBUG_MOVIE
    qDebug() << "max_frames is <= 1, using static duration";
#endif
  }
  if (duration > 0 && cull_image == true)
    shfx_timer->start(duration);
  play();
#ifdef DEBUG_MOVIE
  qDebug() << max_frames << "Setting image to " << image_path
           << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif
}

void CharLayer::play()
{
  play_frame_effect(frame);
  AOLayer::play();
}

void AOLayer::play()
{
  if (max_frames <= 1) {
    if (play_once)
      ticker->start(tick_ms);
    else
      this->freeze();
  }
  else
    ticker->start(this->get_frame_delay(movie_delays[frame]));
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
          qDebug() << "Warning: out of bounds" << effects_list[i] << "frame"
                   << f_frame << "out of" << max_frames << "for" << m_emote;
          continue;
        }
        QString f_data = frame_split.at(1);
        if (f_data != "") {
          QString effect = effects_list[i];
          if (effect == "sfx^") // Currently the only frame result that feeds us
                                // data, let's yank it in.
            effect += f_data;
          qDebug() << effect << f_data << "frame" << f_frame << "for"
                   << m_emote;
          movie_effects[f_frame].append(effect);
        }
      }
    }
  }
}

void CharLayer::play_frame_effect(int p_frame)
{
  if (p_frame < max_frames) {
    foreach (QString effect, movie_effects[p_frame]) {
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
  this->freeze();
  this->hide();
}

void AOLayer::freeze()
{
  // aT nO pOiNt Do We WaNt A fRoZeN gIf To DiSpLaY
  ticker->stop();
  preanim_timer->stop();
  shfx_timer->stop();
}

void CharLayer::movie_ticker()
{
  AOLayer::movie_ticker();
  play_frame_effect(frame);
}

void AOLayer::movie_ticker()
{
  ++frame;
  if ((frame >= max_frames) && (max_frames > 1)) {
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
  //  qint64 difference = elapsed - movie_delays[frame];
  if (frame >= movie_frames.size()) {
    movie_frames.append(this->get_pixmap(m_reader.read()));
    movie_delays.append(m_reader.nextImageDelay());
  }

#ifdef DEBUG_MOVIE
  qDebug() << frame << movie_delays[frame]
           << "actual time taken from last frame:" << actual_time.restart();
#endif

  this->set_frame(movie_frames[frame]);
  ticker->setInterval(this->get_frame_delay(movie_delays[frame]));
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
