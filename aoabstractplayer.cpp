#include "aoabstractplayer.hpp"

AOAbstractPlayer::AOAbstractPlayer(QObject *p_parent, AOApplication *p_ao_app)
    : QObject(p_parent), ao_app(p_ao_app)
{}

int AOAbstractPlayer::get_volume()
{
    return m_volume;
}

void AOAbstractPlayer::set_volume(int p_volume)
{
    m_volume = p_volume;

    emit new_volume(m_volume);
}
