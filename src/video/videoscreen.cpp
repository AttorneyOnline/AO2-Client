#include "video/videoscreen.h"

#include <QAudioDevice>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QStyleOptionGraphicsItem>
#include <QUrl>

VideoScreen::VideoScreen(AOApplication *p_ao_app, QGraphicsItem *parent)
    : QGraphicsVideoItem(parent), m_scanned(false), m_video_available(false),
      m_running(false), m_player(new QMediaPlayer(this))
{
  ao_app = p_ao_app;
  m_player->setVideoOutput(this);

  connect(m_player, &QMediaPlayer::hasVideoChanged, this, &VideoScreen::update_video_availability);
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &VideoScreen::check_status);
  connect(m_player, &QMediaPlayer::playbackStateChanged, this, &VideoScreen::check_state);

  update_audio_output();
}

VideoScreen::~VideoScreen()
{}

QString VideoScreen::get_file_name() const
{
  return m_file_name;
}

void VideoScreen::set_file_name(QString p_file_name)
{
  if (m_file_name == p_file_name)
  {
    return;
  }
  stop();
  qInfo() << "loading media file" << p_file_name;
  m_scanned = false;
  m_video_available = false;
  m_file_name = p_file_name;
  if (m_file_name.isEmpty())
  {
    m_scanned = true;
  }
  m_player->setSource(QUrl::fromLocalFile(m_file_name));
}

void VideoScreen::play_character_video(QString p_charname, QString p_video)
{
  QVector<VPath> pathlist { // cursed character path resolution vector
      ao_app->get_character_path(
          p_charname, "videos/" + p_video),
      ao_app->get_character_path(
          p_charname, p_video),
      VPath(p_video) // The path by itself after the above fail
  };

  const QString l_filepath = ao_app->get_asset_path(pathlist);
  if (l_filepath.isEmpty())
  {
    qWarning() << "error: no character media file" << p_charname << p_video;
    finish_playback();
    return;
  }
  QString expand_override =
      ao_app->read_design_ini("expand", l_filepath + ".ini");

  setAspectRatioMode(Qt::KeepAspectRatio);
  if (expand_override != "" && expand_override.startsWith("true")) {
    setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
  }

  set_file_name(l_filepath);
  play();
}

void VideoScreen::play()
{
  stop();
  m_running = true;
  if (!m_scanned)
  {
    return;
  }
  if (!m_video_available)
  {
    finish_playback();
    return;
  }
  start_playback();
}

void VideoScreen::stop()
{
  m_running = false;
  if (m_player->playbackState() != QMediaPlayer::StoppedState) {
    m_player->stop();
  }
}

void VideoScreen::update_video_availability(bool p_video_available)
{
  m_video_available = p_video_available;
}

void VideoScreen::check_status(QMediaPlayer::MediaStatus p_status)
{
  if (m_running)
  {
    switch (p_status)
    {
    case QMediaPlayer::InvalidMedia:
      m_scanned = true;
      qWarning() << "error: media file is invalid:" << m_file_name;
      finish_playback();
      break;

    case QMediaPlayer::NoMedia:
      m_scanned = true;
      finish_playback();
      break;

    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferedMedia:
      m_scanned = true;
      start_playback();
      break;

    case QMediaPlayer::EndOfMedia:
      m_scanned = true;
      finish_playback();
      break;

    default:
      break;
    }
  }
}

void VideoScreen::check_state(QMediaPlayer::PlaybackState p_state)
{
  switch (p_state)
  {
  case QMediaPlayer::PlayingState:
    emit started();
    break;

  case QMediaPlayer::StoppedState:
    if (m_running)
    {
      finish_playback();
    }
    break;

  default:
    break;
  }
}

void VideoScreen::start_playback()
{
  this->show();
  if (m_player->playbackState() == QMediaPlayer::StoppedState) {
    update_audio_output();
    // Rewind when replaying the same source after it reached the end.
    if (m_player->mediaStatus() == QMediaPlayer::EndOfMedia) {
      m_player->setPosition(0);
    }
    m_player->play();
  }
}

void VideoScreen::finish_playback()
{
  stop();
  emit finished();
}

void VideoScreen::update_audio_output()
{
  // In Qt6, QMediaPlayer has a setAudioOutput(QAudioOutput*) method.
  // Usually, you should keep a pointer to the QAudioOutput object
  // as a member variable of your class.

  QAudioOutput *l_audioOutput = m_player->audioOutput();
  if (!l_audioOutput) {
    // If not already set, create one and attach it
    l_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(l_audioOutput);
  }

  // Get the list of all available audio output devices
  const QList<QAudioDevice> l_devices = QMediaDevices::audioOutputs();
  QString targetDeviceName = Options::getInstance().audioOutputDevice();
  bool deviceFound = false;

  for (const QAudioDevice &device : l_devices) {
    // Qt6 uses unique IDs. You can check against device.id()
    // or device.description() depending on what your Options store.
    if (device.id() == targetDeviceName.toUtf8()) {
      l_audioOutput->setDevice(device);
      qDebug() << "Media player changed audio device to"
               << device.description();
      deviceFound = true;
      break;
    }
  }

  if (!deviceFound) {
    qWarning() << "Could not find audio device:" << targetDeviceName;
  }
}

void VideoScreen::set_volume(int p_value)
{
  if (m_player->audioOutput()->volume() == p_value) {
    return;
  }
  m_player->audioOutput()->setVolume(p_value);
}

void VideoScreen::set_muted(bool p_toggle)
{
  m_player->audioOutput()->setMuted(p_toggle);
}
