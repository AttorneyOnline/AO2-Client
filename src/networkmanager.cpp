#include "networkmanager.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"
#include "options.h"

#include <QAbstractSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

NetworkManager::NetworkManager(AOApplication *parent)
    : QObject(parent)
{
  ao_app = parent;

  http = new QNetworkAccessManager(this);
  heartbeat_timer = new QTimer(this);

  QString master_config = Options::getInstance().alternativeMasterserver();
  if (!master_config.isEmpty() && QUrl(master_config).scheme().startsWith("http"))
  {
    qInfo() << "using alternate master server" << master_config;
    ms_baseurl = master_config;
  }

  connect(heartbeat_timer, &QTimer::timeout, this, &NetworkManager::send_heartbeat);
  heartbeat_timer->start(heartbeat_interval);
}

void NetworkManager::get_server_list()
{
  QNetworkRequest req(QUrl(ms_baseurl + "/servers"));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  QNetworkReply *reply = http->get(req);
  connect(reply, &QNetworkReply::finished, this, std::bind(&NetworkManager::ms_request_finished, this, reply));
}

void NetworkManager::ms_request_finished(QNetworkReply *reply)
{
  QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
  if (json.isNull())
  {
    qCritical().noquote() << "Invalid JSON response from" << reply->url();
    reply->deleteLater();
    return;
  }

  qDebug().noquote() << "Got valid response from" << reply->url();

  QVector<ServerInfo> server_list;
  const auto jsonEntries = json.array();
  for (const auto &entryRef : jsonEntries)
  {
    const auto entry = entryRef.toObject();
    ServerInfo server;
    server.address = entry["ip"].toString();
    server.name = entry["name"].toString();
    server.description = entry["description"].toString(tr("No description provided."));
    if (entry.contains("ws_port"))
    {
      server.port = entry["ws_port"].toInt();
    }
    else
    {
      server.port = entry["port"].toInt();
      server.legacy = true;
    }

    if (server.port != 0)
    {
      server_list.append(server);
    }
  }
  ao_app->set_server_list(server_list);

  if (ao_app->is_lobby_constructed())
  {
    ao_app->w_lobby->list_servers();
  }
  reply->deleteLater();
}

QString NetworkManager::get_user_agent() const
{
  return QStringLiteral("AttorneyOnline/%1 (Desktop)").arg(ao_app->get_version_string());
}

void NetworkManager::send_heartbeat()
{
  // Ping the server periodically to tell the MS that you've been playing
  // within a 5 minute window, so that the the number of people playing within
  // that time period can be counted and an accurate player count be displayed.
  // What do I care about your personal information, I really don't want it.
  if (Options::getInstance().playerCountOptout())
  {
    return;
  }

  QNetworkRequest req(QUrl(ms_baseurl + "/playing"));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  http->post(req, QByteArray());
}

void NetworkManager::request_document(MSDocumentType document_type, const std::function<void(QString)> &cb)
{
  const QMap<MSDocumentType, QString> endpoints{// I have to balance an evil with a good
                                                {MSDocumentType::PrivacyPolicy, "/privacy"},
                                                {MSDocumentType::Motd, "/motd"},
                                                {MSDocumentType::ClientVersion, "/version"}};

  const QString &endpoint = endpoints[document_type];
  QNetworkRequest req(QUrl(ms_baseurl + endpoint));
  req.setRawHeader("User-Agent", get_user_agent().toUtf8());

  QString language = Options::getInstance().language();
  if (language.trimmed().isEmpty())
  {
    language = QLocale::system().name();
  }

  req.setRawHeader("Accept-Language", language.toUtf8());

  qDebug().noquote().nospace() << "Getting " << endpoint << ", Accept-Language: " << language;

  QNetworkReply *reply = http->get(req);
  connect(reply, &QNetworkReply::finished, this, [endpoint, cb, reply] {
    QString content = QString::fromUtf8(reply->readAll());
    int http_status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (content.isEmpty() || http_status != 200)
    {
      qDebug().noquote().nospace() << "Failed to get " << endpoint << " (" << reply->errorString() << ") "
                                   << "(http status " << http_status << ")";
      content = QString();
    }
    cb(content);
    reply->deleteLater();
  });
}

void NetworkManager::connect_to_server(ServerInfo server)
{
  disconnect_from_server();

  qInfo().noquote() << QObject::tr("Connecting to %1").arg(server.toString());
  m_connection = new WebSocketConnection(ao_app, this);

  connect(m_connection, &WebSocketConnection::connectedToServer, ao_app, &AOApplication::server_connected);
  connect(m_connection, &WebSocketConnection::disconnectedFromServer, ao_app, &AOApplication::server_disconnected);
  connect(m_connection, &WebSocketConnection::errorOccurred, this, [](QString error) { qCritical() << "Connection error:" << error; });
  connect(m_connection, &WebSocketConnection::receivedPacket, this, &NetworkManager::handle_server_packet);

  m_connection->connectToServer(server);
}

void NetworkManager::disconnect_from_server()
{
  if (m_connection)
  {
    m_connection->disconnectFromServer();
    m_connection->deleteLater();
    m_connection = nullptr;
  }
}

void NetworkManager::ship_server_packet(AOPacket packet)
{
  if (!m_connection)
  {
    qCritical() << "Failed to ship packet; no connection.";
    return;
  }

  if (!m_connection->isConnected())
  {
    qCritical() << "Failed to ship packet; not connected.";
    return;
  }
#ifdef NETWORK_DEBUG
  qInfo().noquote() << "Sending packet:" << packet.toString();
#endif
  m_connection->sendPacket(packet);
}

void NetworkManager::join_to_server()
{
  ship_server_packet(AOPacket("askchaa"));
}

void NetworkManager::handle_server_packet(AOPacket packet)
{
#ifdef NETWORK_DEBUG
  qInfo().noquote() << "Received packet:" << packet.toString();
#endif
  ao_app->server_packet_received(packet);
}
