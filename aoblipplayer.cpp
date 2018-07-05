#include "aoblipplayer.h"

#include <string.h>

#include <QDebug>

AOBlipPlayer::AOBlipPlayer(QObject *p_parent, AOApplication *p_ao_app)
    : AOAbstractPlayer(p_parent, p_ao_app)
{}

void AOBlipPlayer::set_file(QString p_file)
{
    m_file = ao_app->get_sounds_path() + QString("sfx-blip%1.wav").arg(p_file).toLower();
}

void AOBlipPlayer::play()
{
    try {
        AOBassHandle *handle = new AOBassHandle(m_file, true, this);
        connect(this, &AOBlipPlayer::new_volume, handle, &AOBassHandle::set_volume);
        handle->set_volume(get_volume());
        handle->play();
    } catch (const std::exception &e_exception) {
        qDebug() << e_exception.what();
    }
}
