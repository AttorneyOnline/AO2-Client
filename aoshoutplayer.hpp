#ifndef AOSHOUTPLAYER_HPP
#define AOSHOUTPLAYER_HPP

#include "aoabstractplayer.hpp"

class AOShoutPlayer : public AOAbstractPlayer
{
    Q_OBJECT

public:
    AOShoutPlayer(QObject *p_parent, AOApplication *p_ao_app);

    void play(QString p_name, QString p_char);
    void stop();
};

#endif // AOSHOUTPLAYER_HPP
