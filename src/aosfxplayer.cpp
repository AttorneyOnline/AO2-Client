#include "aosfxplayer.h"
#include "file_functions.h"

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app): QObject()
{
  m_parent = parent;
  ao_app = p_ao_app;
  sfx_loop_timer = new QTimer(this);
  sfx_loop_timer->setSingleShot(true);
  connect(sfx_loop_timer, SIGNAL(timeout()), this, SLOT(restart_loop()));
}

void AOSfxPlayer::play(QString p_sfx, QString p_char, QString shout)
{
  BASS_ChannelStop(m_stream);
  
  QString misc_path = "";
  QString char_path = "";
  QString sound_path = ao_app->get_sounds_path(p_sfx);

  if (shout != "")
    misc_path = ao_app->get_base_path() + "misc/" + shout + "/" + p_sfx;
  if (p_char != "")
    char_path = ao_app->get_character_path(p_char, p_sfx);

  QString f_path;

  if (file_exists(char_path))
      f_path = char_path;
  else if (file_exists(misc_path))
    f_path = misc_path;
  else
    f_path = sound_path;
  BASS_ChannelStop(m_stream);
  m_stream = BASS_StreamCreateFile(FALSE, f_path.utf16(), 0, 0, BASS_STREAM_AUTOFREE | BASS_UNICODE | BASS_ASYNCFILE);

  set_volume(m_volume);

  if (ao_app->get_audio_output_device() != "default")
    BASS_ChannelSetDevice(m_stream, BASS_GetDevice());
  BASS_ChannelPlay(m_stream, false);
  sfx_loop_timer->stop();
  QWORD len=BASS_ChannelGetLength(m_stream, BASS_POS_BYTE); // the length in bytes
  double time=BASS_ChannelBytes2Seconds(m_stream, len); // the length in seconds
  if(time > 0 && looping_sfx && ao_app->get_looping_sfx())
  {
    sfx_loop_timer->start(time*1000);
  }
}

void AOSfxPlayer::setLooping(bool is_looping)
{
    this->looping_sfx = is_looping;
}

void AOSfxPlayer::stop()
{
  BASS_ChannelStop(m_stream);
  sfx_loop_timer->stop();
}

void AOSfxPlayer::restart_loop()
{
    if(ao_app->get_looping_sfx() && looping_sfx)
    {
        QWORD len=BASS_ChannelGetLength(m_stream, BASS_POS_BYTE); // the length in bytes
        double time=BASS_ChannelBytes2Seconds(m_stream, len); // the length in seconds
        sfx_loop_timer->start(time*1000);
        BASS_ChannelPlay(m_stream, true);
    }
}

void AOSfxPlayer::set_volume(int p_value)
{
  m_volume = p_value;
  float volume = p_value / 100.0f;
  BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume);
}
