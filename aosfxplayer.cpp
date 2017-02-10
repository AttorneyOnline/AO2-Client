#include "aosfxplayer.h"

#include <string.h>

AOSfxPlayer::AOSfxPlayer(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;
  ao_app = p_ao_app;

  std::basic_string<char> path = ao_app->get_sounds_path().toStdString() + "sfx-squee.wav";

  m_stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, 0);
}
