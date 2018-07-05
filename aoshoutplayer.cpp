#include "aoshoutplayer.hpp"

#include <QDebug>

AOShoutPlayer::AOShoutPlayer(QObject *p_parent, AOApplication *p_ao_app)
    : AOAbstractPlayer(p_parent, p_ao_app)
{}

void AOShoutPlayer::play(QString p_name, QString p_char)
{
    QString f_file = ao_app->get_character_path(p_char) + p_name.toLower();

    try {
        AOBassHandle *handle = new AOBassHandle(f_file, true, this);
        connect(this, &AOShoutPlayer::new_volume, handle, &AOBassHandle::set_volume);
        connect(this, &AOShoutPlayer::stopping, handle, &AOBassHandle::stop);
        handle->set_volume(get_volume());
        handle->play();
    } catch(const std::exception &e_exception) {
        qDebug() << e_exception.what();
    }
}

void AOShoutPlayer::stop()
{
    emit stopping();
}
