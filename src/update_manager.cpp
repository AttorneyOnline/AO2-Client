#include "update_manager.h"

update_manager::update_manager(AOApplication *parent) : QObject(parent)
{
    ao_app = parent;

}

update_manager::~update_manager(){}

void update_manager::get_latest_update()
{
    QUrl url("http://127.0.0.1:5000/updates");
    QUrlQuery query;
    query.addQueryItem("cver","19");
    url.setQuery(query.query());
    request.setUrl(url);


    manager->get(request);

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
    return;
}

void update_manager::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << answer;
}
