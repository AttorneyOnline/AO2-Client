#include "aomusicplayer.h"

#include <string.h>

#include <QDebug>

AOMusicPlayer::AOMusicPlayer(QObject *p_parent, AOApplication *p_ao_app)
    : AOAbstractPlayer(p_parent, p_ao_app)
{}

void AOMusicPlayer::play(QString p_file)
{
    QString f_file = ao_app->get_music_path(p_file);

    stop();

    m_file = f_file;

    try { // create new song
        AOBassHandle *handle = new AOBassHandle(m_file, false, this);
        connect(this, &AOMusicPlayer::new_volume, handle, &AOBassHandle::set_volume);
        connect(this, &AOMusicPlayer::stopping, handle, &AOBassHandle::stop);

        // delete previous
        if (m_handle)
            delete m_handle;

        m_handle = handle;
        m_handle->set_volume(get_volume());
        m_handle->play();
    } catch(const std::exception &e_exception) {
        qDebug() << e_exception.what();
    }
}

void AOMusicPlayer::stop()
{
    emit stopping();
}
