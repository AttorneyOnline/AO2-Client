#include "update_manager.h"

update_manager::update_manager(AOApplication *parent) : QObject(parent)
{
    ao_app = parent;

}
QString get_latest_update()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://qt-project.org"));
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

    QNetworkAccessManager *manager = new QNetworkAccessManager(manager);

    manager->get(QNetworkRequest(QUrl("http://AmazingUpdateServer.org")));
    QNetworkReply *reply = manager->get(request);
    return "Yes this works totally";
}
