#ifndef AOBLIPPLAYER_H
#define AOBLIPPLAYER_H

#if defined(BASSAUDIO)
#include "bass.h"
#include "bassopus.h"
#elif defined(QTAUDIO)
#include <QSoundEffect>
#endif

#include "aoapplication.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QWidget>
#include <string.h>

class AOBlipPlayer {
public:
    AOBlipPlayer(QWidget *parent, AOApplication *p_ao_app);

    void set_blips(QString p_sfx);
    void blip_tick();
    void set_volume(qreal p_volume);

    int m_cycle = 0;

private:
    const int max_blip_ms = 60;

    QWidget *m_parent;
    AOApplication *ao_app;
    qreal m_volume;
    QElapsedTimer delay;

    void set_volume_internal(qreal p_volume);

#if defined(BASSAUDIO)
    HSTREAM m_stream_list[5];
#elif defined(QTAUDIO)
    QSoundEffect m_blips;
#endif
};

#endif // AOBLIPPLAYER_H
