#include "networkmanager.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

NetworkManager::NetworkManager(AOApplication *parent) : QObject(parent)
{
  ao_app = parent;

  server_socket = new QTcpSocket(this);
  http = new QNetworkAccessManager(this);
  heartbeat_timer = new QTimer(this);

  connect(server_socket, SIGNAL(readyRead()), this,
                   SLOT(handle_server_packet()));
  connect(server_socket, SIGNAL(disconnected()), ao_app,
                   SLOT(server_disconnected()));

  QString master_config =
      ao_app->configini->value("master", "").value<QString>();
  if (!master_config.isEmpty())
    ms_baseurl = master_config;

  connect(heartbeat_timer, &QTimer::timeout, this, &NetworkManager::send_heartbeat);
  heartbeat_timer->start(heartbeat_interval);
}

NetworkManager::~NetworkManager() {}

void NetworkManager::get_server_list(const std::function<void()> &cb)
{
  QNetworkRequest req(QUrl(ms_baseurl + "/servers"));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  QNetworkReply *reply = http->get(req);
  connect(reply, &QNetworkReply::finished,
          this, std::bind(&NetworkManager::ms_request_finished, this, reply, cb));
}

void NetworkManager::ms_request_finished(QNetworkReply *reply,
                                         const std::function<void()> &cb)
{
  QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
  if (json.isNull()) {
    qCritical().noquote() << "Invalid JSON response from" << reply->url();
    reply->deleteLater();
    return;
  }

  qDebug().noquote() << "Got valid response from" << reply->url();

  QVector<server_type> server_list;
  const auto jsonEntries = json.array();
  for (const auto &entryRef : jsonEntries) {
    const auto entry = entryRef.toObject();
    server_type server;
    server.ip = entry["ip"].toString();
    server.port = entry["port"].toInt();
    server.name = entry["name"].toString();
    server.desc = entry["description"].toString(tr("No description provided."));
    server_list.append(server);
  }
  ao_app->set_server_list(server_list);

  cb();

  reply->deleteLater();
}

void NetworkManager::send_heartbeat()
{
  // Ping the server periodically to tell the MS that you've been playing
  // within a 5 minute window, so that the the number of people playing within
  // that time period can be counted and an accurate player count be displayed.
  // What do I care about your personal information, I really don't want it.
  if (ao_app->get_player_count_optout())
    return;

  QNetworkRequest req(QUrl(ms_baseurl + "/playing"));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  http->post(req, QByteArray());
}

void NetworkManager::request_document(MSDocumentType document_type,
                                      const std::function<void(QString)> &cb)
{
  const QMap<MSDocumentType, QString> endpoints {
    // I have to balance an evil with a good
    { MSDocumentType::PrivacyPolicy, "/privacy" },
    { MSDocumentType::Motd, "/motd" },
    { MSDocumentType::ClientVersion, "/version" }
  };

  const QString &endpoint = endpoints[document_type];
  QNetworkRequest req(QUrl(ms_baseurl + endpoint));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  QString language =
      ao_app->configini->value("language").toString();
  if (language.trimmed().isEmpty())
    language = QLocale::system().name();

  req.setRawHeader("Accept-Language", language.toUtf8());

  qDebug().noquote().nospace()
      << "Getting " << endpoint << ", Accept-Language: " << language;

  QNetworkReply *reply = http->get(req);
  connect(reply, &QNetworkReply::finished, this, [endpoint, cb, reply] {
    QString content = QString::fromUtf8(reply->readAll());
    int http_status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (content.isEmpty() || http_status != 200) {
      qDebug().noquote().nospace()
          << "Failed to get " << endpoint << " (" << reply->errorString() << ") "
          << "(http status " << http_status << ")";
      content = QString();
    } cb(content);
    reply->deleteLater();
  });
}

void NetworkManager::connect_to_server(server_type p_server)
{
  server_socket->close();
  server_socket->abort();

  server_socket->connectToHost(p_server.ip, p_server.port);
}

void NetworkManager::ship_server_packet(QString p_packet)
{
  server_socket->write(p_packet.toUtf8());
}

void NetworkManager::handle_server_packet()
{
  QByteArray buffer = server_socket->readAll();
  QString in_data = QString::fromUtf8(buffer, buffer.size());

  if (!in_data.endsWith("%")) {
    partial_packet = true;
    temp_packet += in_data;
    return;
  }

  else {
    if (partial_packet) {
      in_data = temp_packet + in_data;
      temp_packet = "";
      partial_packet = false;
    }
  }

  const QStringList packet_list = in_data.split("%", QString::SkipEmptyParts);

  for (const QString &packet : packet_list) {
    AOPacket *f_packet = new AOPacket(packet);

    ao_app->server_packet_received(f_packet);
  }
}
