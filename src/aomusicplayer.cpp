#include "aomusicplayer.h"
#include "options.h"

#include "bass.h"
#include "file_functions.h"

AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    BASS_ChannelStop(m_stream_list[n_stream]);
  }
}

QString AOMusicPlayer::play(QString p_song, int channel, bool loop,
                         int effect_flags)
{
  channel = channel % m_channelmax;
  if (channel < 0) // wtf?
    return "[ERROR] Invalid Channel";
  unsigned int flags = BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE |
                       BASS_UNICODE | BASS_ASYNCFILE;
  unsigned int streaming_flags = BASS_STREAM_AUTOFREE;
  if (loop) {
    flags |= BASS_SAMPLE_LOOP;
    streaming_flags |= BASS_SAMPLE_LOOP;
  }
  QString f_path = p_song;
  DWORD newstream;
  if (f_path.startsWith("http")) {

    if (!Options::getInstance().streamingEnabled()) {
        BASS_ChannelStop(m_stream_list[channel]);
        return QObject::tr("[MISSING] Streaming disabled.");
    }
    newstream = BASS_StreamCreateURL(f_path.toStdString().c_str(), 0, streaming_flags, nullptr, 0);
  }
  else {
    f_path = ao_app->get_real_path(ao_app->get_music_path(p_song));
    if (f_path.endsWith(".mo3") || f_path.endsWith(".xm") || f_path.endsWith(".mod") || f_path.endsWith(".s3m") || f_path.endsWith(".it") || f_path.endsWith(".mtm") || f_path.endsWith(".umx") )
      newstream = BASS_MusicLoad(FALSE,f_path.utf16(), 0, 0, flags, 1);
    else
      newstream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, flags);
  }

  int error_code = BASS_ErrorGetCode();

  if (Options::getInstance().audioOutputDevice() != "default")
    BASS_ChannelSetDevice(m_stream_list[channel], BASS_GetDevice());

  QString d_path = f_path + ".txt";

  loop_start[channel] = 0;
  loop_end[channel] = 0;
  if (loop && file_exists(d_path)) // Contains loop/etc. information file
  {
    QStringList lines = ao_app->read_file(d_path).split("\n");
    foreach (QString line, lines) {
      QStringList args = line.split("=");
      if (args.size() < 2)
        continue;
      QString arg = args[0].trimmed();

      float sample_rate;
      BASS_ChannelGetAttribute(newstream, BASS_ATTRIB_FREQ, &sample_rate);

      // Grab number of bytes for sample size
      int sample_size = 16 / 8;

      // number of channels (stereo/mono)
      int num_channels = 2;

      // Calculate the bytes for loop_start/loop_end to use with the sync proc
      QWORD bytes = static_cast<QWORD>(args[1].trimmed().toUInt() *
                                       sample_size * num_channels);
      if (arg == "loop_start")
        loop_start[channel] = bytes;
      else if (arg == "loop_length")
        loop_end[channel] = loop_start[channel] + bytes;
      else if (arg == "loop_end")
        loop_end[channel] = bytes;
    }
    qDebug() << "Found data file for song" << p_song << "length"
             << BASS_ChannelGetLength(newstream, BASS_POS_BYTE) << "loop start"
             << loop_start[channel] << "loop end" << loop_end[channel];
  }

  if (BASS_ChannelIsActive(m_stream_list[channel]) == BASS_ACTIVE_PLAYING) {
    DWORD oldstream = m_stream_list[channel];

    if (effect_flags & SYNC_POS) {
      BASS_ChannelLock(oldstream, true);
      // Sync it with the new sample
      BASS_ChannelSetPosition(newstream,
                              BASS_ChannelGetPosition(oldstream, BASS_POS_BYTE),
                              BASS_POS_BYTE);
      BASS_ChannelLock(oldstream, false);
    }

    if ((effect_flags & FADE_OUT) && m_volume[channel] > 0) {
      // Fade out the other sample and stop it (due to -1)
      BASS_ChannelSlideAttribute(oldstream, BASS_ATTRIB_VOL | BASS_SLIDE_LOG,
                                 -1, 4000);
    }
    else
      BASS_ChannelStop(
          oldstream); // Stop the sample since we don't need it anymore
  }
  else
    BASS_ChannelStop(m_stream_list[channel]);

  m_stream_list[channel] = newstream;
  BASS_ChannelPlay(newstream, false);
  if (effect_flags & FADE_IN) {
    // Fade in our sample
    BASS_ChannelSetAttribute(newstream, BASS_ATTRIB_VOL, 0);
    BASS_ChannelSlideAttribute(newstream, BASS_ATTRIB_VOL,
                               static_cast<float>(m_volume[channel] / 100.0f),
                               1000);
  }
  else
    this->set_volume(m_volume[channel], channel);

  BASS_ChannelSetSync(newstream, BASS_SYNC_DEV_FAIL, 0,
                      ao_app->BASSreset, 0);

  this->set_looping(loop, channel); // Have to do this here due to any
                                    // crossfading-related changes, etc.

  bool is_stop = (p_song == "~stop.mp3");
  QString p_song_clear = QUrl(p_song).fileName();
  p_song_clear = p_song_clear.left(p_song_clear.lastIndexOf('.'));

  if (is_stop) {
    return QObject::tr("None");
  }

  if (error_code == BASS_ERROR_HANDLE) { // Cheap hack to see if file missing
    return QObject::tr("[MISSING] %1").arg(p_song_clear);
  }

  if (p_song.startsWith("http") && channel == 0) {
    return QObject::tr("[STREAM] %1").arg(p_song_clear);
  }

  if (channel == 0)
    return p_song_clear;

  return "";
}

void AOMusicPlayer::stop(int channel)
{
  BASS_ChannelStop(m_stream_list[channel]);
}

void AOMusicPlayer::set_muted(bool toggle)
{
  m_muted = toggle;
  // Update all volume based on the mute setting
  for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
    set_volume(m_volume[n_stream], n_stream);
  }
}

void AOMusicPlayer::set_volume(int p_value, int channel)
{
  m_volume[channel] = p_value;
  // If muted, volume will always be 0
  float volume = (m_volume[channel] / 100.0f) * !m_muted;
  if (channel < 0) {
    for (int n_stream = 0; n_stream < m_channelmax; ++n_stream) {
      BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL,
                               volume);
    }
  }
  else {
    BASS_ChannelSetAttribute(m_stream_list[channel], BASS_ATTRIB_VOL, volume);
  }
}

void CALLBACK loopProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
  Q_UNUSED(handle);
  Q_UNUSED(data);
  QWORD loop_start = *(static_cast<unsigned *>(user));
  BASS_ChannelLock(channel, true);
  BASS_ChannelSetPosition(channel, loop_start, BASS_POS_BYTE);
  BASS_ChannelLock(channel, false);
}

void AOMusicPlayer::set_looping(bool loop_song, int channel)
{
  if (!loop_song) {
    if (BASS_ChannelFlags(m_stream_list[channel], 0, 0) & BASS_SAMPLE_LOOP)
      BASS_ChannelFlags(m_stream_list[channel], 0,
                        BASS_SAMPLE_LOOP); // remove the LOOP flag
    BASS_ChannelRemoveSync(m_stream_list[channel], loop_sync[channel]);
    loop_sync[channel] = 0;
    return;
  }

  BASS_ChannelFlags(m_stream_list[channel], BASS_SAMPLE_LOOP,
                    BASS_SAMPLE_LOOP); // set the LOOP flag
  if (loop_sync[channel] != 0) {
    BASS_ChannelRemoveSync(m_stream_list[channel],
                           loop_sync[channel]); // remove the sync
    loop_sync[channel] = 0;
  }

  if (loop_start[channel] < loop_end[channel])
  {
    //Loop when the endpoint is reached.
    loop_sync[channel] = BASS_ChannelSetSync(
        m_stream_list[channel], BASS_SYNC_POS | BASS_SYNC_MIXTIME,
        loop_end[channel], loopProc, &loop_start[channel]);
  }
  else
  {
    //Loop when the end of the file is reached.
    loop_sync[channel] = BASS_ChannelSetSync(
        m_stream_list[channel], BASS_SYNC_END | BASS_SYNC_MIXTIME,
        0, loopProc, &loop_start[channel]);
  }
}
