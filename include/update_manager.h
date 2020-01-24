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
#include <QNetworkReply>
#include <QUrlQuery>

class update_manager : public QObject
{
    Q_OBJECT
public:
    update_manager(AOApplication *parent);
    ~update_manager();
    AOApplication *ao_app;
    bool abcd = false;
    void get_latest_update();
private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // UPDATE_MANAGER_H
