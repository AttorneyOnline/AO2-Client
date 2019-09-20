#include "aomusicplayer.h"

#ifdef BASSAUDIO
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  for (int n_stream = 0 ; n_stream < m_channelmax ; ++n_stream)
  {
    BASS_ChannelStop(m_stream_list[n_stream]);
  }
}

void AOMusicPlayer::play(QString p_song, int channel, bool crossfade)
{
  channel = channel % m_channelmax;
  if (channel < 0) //wtf?
      return;
  QString f_path = ao_app->get_music_path(p_song);
//  QString d_path = f_path + ".txt";

//  if (file_exists(d_path)) //Contains loop/etc. information file
//  {
//    QStringList lines = ao_app->read_file(d_path).split("\n");
//    foreach (QString line, lines)
//    {
//      QStringList args = line.split("=");
//      if (args.size() < 2)
//        continue;
//      QString arg = args[0].trimmed();
//      if (arg == "loop_start")
//        loop_start = args[1].trimmed().toUInt();
//      else if (arg == "loop_end")
//        loop_end = args[1].trimmed().toUInt();
//    }
//    qDebug() << "Found data file for song" << p_song << loop_start << loop_end;
//  }

  unsigned int flags = BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE;
  if (m_looping)
    flags |= BASS_SAMPLE_LOOP;
  DWORD newstream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, flags);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream_list[channel], BASS_GetDevice());

  if (crossfade)
  {
    DWORD oldstream = m_stream_list[channel];
    //Mute the new sample
    BASS_ChannelSetAttribute(newstream, BASS_ATTRIB_VOL, 0);
    //Crossfade time
    if (BASS_ChannelIsActive(oldstream) == BASS_ACTIVE_PLAYING)
    {
      //Fade out the other sample and stop it
      BASS_ChannelSlideAttribute(oldstream, BASS_ATTRIB_VOL|BASS_SLIDE_LOG, -1, 5000);
      BASS_ChannelLock(oldstream, true);
      //Sync it with the new sample
      BASS_ChannelSetPosition(newstream, BASS_ChannelGetPosition(oldstream, BASS_POS_BYTE), BASS_POS_BYTE);
      BASS_ChannelLock(oldstream, false);
    }
    //Start it
    BASS_ChannelPlay(newstream, false);
    //Fade in our sample
    BASS_ChannelSlideAttribute(newstream, BASS_ATTRIB_VOL, static_cast<float>(m_volume / 100.0f), 1000);

    m_stream_list[channel] = newstream;
  }
  else
  {
    BASS_ChannelStop(m_stream_list[channel]);
    m_stream_list[channel] = newstream;
    BASS_ChannelPlay(m_stream_list[channel], false);
    this->set_volume(m_volume, channel);
  }

  this->set_looping(m_looping); //Have to do this here due to any crossfading-related changes, etc.
}

void AOMusicPlayer::stop(int channel)
{
    BASS_ChannelStop(m_stream_list[channel]);
}

void AOMusicPlayer::set_volume(int p_value, int channel)
{
  m_volume = p_value;
  float volume = m_volume / 100.0f;
  if (channel < 0)
  {
    for (int n_stream = 0 ; n_stream < m_channelmax ; ++n_stream)
    {
      BASS_ChannelSetAttribute(m_stream_list[n_stream], BASS_ATTRIB_VOL, volume);
    }
  }
  else
  {
    BASS_ChannelSetAttribute(m_stream_list[channel], BASS_ATTRIB_VOL, volume);
  }
}

//void CALLBACK loopProc(HSYNC handle, DWORD channel, DWORD data, void *user)
//{
//  AOMusicPlayer *self= static_cast<AOMusicPlayer*>(user);
//  qDebug() << BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
//  BASS_ChannelSetPosition(channel, self->loop_start, BASS_POS_BYTE);
//}

void AOMusicPlayer::set_looping(bool toggle, int channel)
{
  m_looping = toggle;
  if (BASS_ChannelFlags(m_stream_list[channel], 0, 0) & BASS_SAMPLE_LOOP)
  {
      if (m_looping == false)
        BASS_ChannelFlags(m_stream_list[channel], 0, BASS_SAMPLE_LOOP); // remove the LOOP flag
//      BASS_ChannelRemoveSync(m_stream_list[channel], loop_sync);
  }
  else
  {
      if (m_looping == true)
        BASS_ChannelFlags(m_stream_list[channel], BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP); // set the LOOP flag
//      if (loop_end > 0 && loop_end < BASS_ChannelGetLength(m_stream_list[channel], BASS_POS_BYTE))
//        loop_sync = BASS_ChannelSetSync(m_stream_list[channel], BASS_SYNC_POS | BASS_SYNC_MIXTIME, loop_end, loopProc, this);
  }
}
#elif defined(QTAUDIO)
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{
  m_player.stop();
}

void AOMusicPlayer::play(QString p_song)
{
  m_player.stop();

  QString f_path = ao_app->get_music_path(p_song);

  m_player.setMedia(QUrl::fromLocalFile(f_path));

  this->set_volume(m_volume);

  m_player.play();
}

void AOMusicPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  m_player.setVolume(m_volume);
}
#else
AOMusicPlayer::AOMusicPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOMusicPlayer::~AOMusicPlayer()
{

}

void AOMusicPlayer::play(QString p_song)
{

}

void AOMusicPlayer::set_volume(int p_value)
{

}
#endif
