#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#include "aoabstractplayer.hpp"

class AOBlipPlayer : public AOAbstractPlayer
{
    Q_OBJECT

public:
    AOBlipPlayer(QObject *p_parent, AOApplication *p_ao_app);

    void set_file(QString p_file);

public slots:
    void play();

private:
    QString m_file;
};

#endif // AOBLIPPLAYER_H
