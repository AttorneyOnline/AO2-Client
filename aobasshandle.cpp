#include "aobasshandle.hpp"

AOBassHandle::AOBassHandle(QObject *p_parent)
  : QObject(p_parent)
{}

AOBassHandle::AOBassHandle(QString p_file, bool p_suicide, QObject *p_parent) noexcept(false)
  : QObject(p_parent)
{
  set_file(p_file, p_suicide);
}

AOBassHandle::~AOBassHandle()
{
  if (m_handle && m_sync)
  {
    BASS_ChannelRemoveSync(m_handle, m_sync);
  }

  // nothing will go wrong if the handle isn't initialized, I promise...!
  if (m_handle)
  {
    BASS_StreamFree(m_handle);
  }
}

void AOBassHandle::suicide()
{
  // safety first
  disconnect();

  // suicide note
  emit body_discovery();

  delete this;
}

QString AOBassHandle::get_file()
{
  return m_file;
}

void AOBassHandle::set_file(QString p_file, bool p_suicide) noexcept(false)
{
  if (m_handle)
    return; // already created

  m_file = p_file;

  // create a handle based on the file
  m_handle = BASS_StreamCreateFile(FALSE, m_file.utf16(), 0, 0, BASS_UNICODE|BASS_ASYNCFILE);
  if (!m_handle)
    throw AOException(QString("%1 could not be initialized to play").arg(p_file));

  m_sync = BASS_ChannelSetSync(m_handle, BASS_SYNC_END, 0, &AOBassHandle::static_sync, this);
  if (!m_sync)
  {
    // free stream since we can't sync
    BASS_StreamFree(m_handle);

    throw AOException(QString("could not set sync for %1").arg(m_file));
  }

  m_suicide = p_suicide;
  if (m_suicide)
    connect(this, &AOBassHandle::stopped, this, &AOBassHandle::suicide);
}

void AOBassHandle::set_volume(int p_volume)
{
  BASS_ChannelSetAttribute(m_handle, BASS_ATTRIB_VOL, p_volume/100.0f);
}

void AOBassHandle::play()
{
  BASS_ChannelPlay(m_handle, FALSE);
}

void AOBassHandle::stop()
{
  BASS_ChannelStop(m_handle);
}

void CALLBACK AOBassHandle::static_sync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
  if (auto self = static_cast<AOBassHandle*>(user))
  {
    self->sync(data);
  }
}

void AOBassHandle::sync(DWORD data)
{
  Q_UNUSED(data)

  emit stopped();
}
