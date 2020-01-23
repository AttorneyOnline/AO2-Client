#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include "aoapplication.h"
#include "file_functions.h"
#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QColor>
#include <QSettings>
#include <QNetworkAccessManager>

class update_manager : public QObject
{
    Q_OBJECT
public:
    update_manager(AOApplication *parent);
    //~update_manager();
    AOApplication *ao_app;
    QString get_latest_update();
};

#endif // UPDATE_MANAGER_H
