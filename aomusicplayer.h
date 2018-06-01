#ifndef AOMUSICPLAYER_H
#define AOMUSICPLAYER_H

#include "aoabstractplayer.hpp"

class AOMusicPlayer : public AOAbstractPlayer
{
    Q_OBJECT

public:
    AOMusicPlayer(QObject *p_parent, AOApplication *p_ao_app);

    void play(QString p_file);
    void stop();

private:
    AOBassHandle* m_handle = nullptr;
    QString       m_file;
};

#endif // AOMUSICPLAYER_H
