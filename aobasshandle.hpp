#ifndef AOBASSHANDLE_HPP
#define AOBASSHANDLE_HPP

#include <QObject>
#include <QString>

#include <bass.h>

#include "aoexception.hpp"

/**
 * @brief The AOBassHandle is a small class to play sounds.
 * The class may destroy itself once the audio provided is done playing.
 */

class AOBassHandle : public QObject
{
    Q_OBJECT

public:
    AOBassHandle(QObject *p_parent = nullptr);
    AOBassHandle(QString p_file, bool p_suicide, QObject *p_parent = nullptr) noexcept(false);
    ~AOBassHandle();

    QString get_file();
    void set_file(QString p_file, bool p_suicide = false) noexcept(false);

    // static
    static void CALLBACK static_sync(HSYNC handle, DWORD channel, DWORD data, void *user);

public slots:
    void play();
    void stop();

    void set_volume(int p_volume);

signals:
    void stopped();
    void body_discovery();

private:
    QString m_file;
    HSTREAM m_handle  = 0;
    HSYNC   m_sync    = 0;
    bool    m_suicide = false;

    void sync(DWORD data);

private slots:
    void suicide();
};

#endif // AOBASSHANDLE_HPP
