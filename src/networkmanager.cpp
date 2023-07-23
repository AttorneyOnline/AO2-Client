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

NetworkManager::NetworkManager(AOApplication *parent) : QObject(parent)
{
  ao_app = parent;

  http = new QNetworkAccessManager(this);
  heartbeat_timer = new QTimer(this);
  stream = new QNetworkAccessManager(this);

  QString master_config =
      Options::getInstance().alternativeMasterserver();
  if (!master_config.isEmpty() && QUrl(master_config).scheme().startsWith("http")) {
    qInfo() << "using alternate master server" << master_config;
    ms_baseurl = master_config;
  }

  connect(heartbeat_timer, &QTimer::timeout, this, &NetworkManager::send_heartbeat);
  heartbeat_timer->start(heartbeat_interval);
  connect(stream, &QNetworkAccessManager::finished, this, &NetworkManager::image_reply_finished, Qt::UniqueConnection);

}

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
    server.name = entry["name"].toString();
    server.desc = entry["description"].toString(tr("No description provided."));
    if (entry["ws_port"].isDouble()) {
      server.socket_type = WEBSOCKETS;
      server.port = entry["ws_port"].toInt();
    } else {
      server.socket_type = TCP;
      server.port = entry["port"].toInt();
    }
    if (server.port != 0) {
        server_list.append(server);
    }
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
  if (Options::getInstance().playerCountOptout())
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
      Options::getInstance().language();
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
    }
    cb(content);
    reply->deleteLater();
  });
}

void NetworkManager::connect_to_server(server_type p_server)
{
  disconnect_from_server();

  qInfo().nospace().noquote() << "connecting to " << p_server.ip << ":"
                              << p_server.port;

  switch (p_server.socket_type) {
  default:
    p_server.socket_type = TCP;
    [[fallthrough]];
  case TCP:
    qInfo() << "using TCP backend";
    server_socket.tcp = new QTcpSocket(this);

    connect(server_socket.tcp, &QAbstractSocket::connected, this, [] {
      qDebug() << "established connection to server";
    });
    connect(server_socket.tcp, &QIODevice::readyRead, this, [this] {
      handle_server_packet(QString::fromUtf8(server_socket.tcp->readAll()));
    });
    connect(server_socket.tcp, &QAbstractSocket::disconnected, ao_app,
            &AOApplication::server_disconnected);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(server_socket.tcp, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [this] {
#else
    connect(server_socket.tcp, &QAbstractSocket::errorOccurred, this, [this] {
#endif
      qCritical() << "TCP socket error:" << server_socket.tcp->errorString();
    });

    server_socket.tcp->connectToHost(p_server.ip, p_server.port);
    break;
  case WEBSOCKETS:
    qInfo() << "using WebSockets backend";
    server_socket.ws = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    connect(server_socket.ws, &QWebSocket::connected, this, [] {
      qDebug() << "established connection to server";
    });
    connect(server_socket.ws, &QWebSocket::textMessageReceived, this,
            &NetworkManager::handle_server_packet);
    connect(server_socket.ws, &QWebSocket::disconnected, ao_app,
            &AOApplication::server_disconnected);
    connect(server_socket.ws, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, [this] {
      qCritical() << "WebSockets error:" << server_socket.ws->errorString();
    });

    QUrl url;
    url.setScheme("ws");
    url.setHost(p_server.ip);
    url.setPort(p_server.port);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, get_user_agent());
    server_socket.ws->open(req);
    break;
  }

  connected = true;
  active_connection_type = p_server.socket_type;
}

void NetworkManager::join_to_server()
{
    ship_server_packet(AOPacket("askchaa").to_string());
}

void NetworkManager::disconnect_from_server()
{
  if (!connected)
    return;

  switch (active_connection_type) {
  case TCP:
    server_socket.tcp->close();
    server_socket.tcp->deleteLater();
    break;
  case WEBSOCKETS:
    server_socket.ws->close(QWebSocketProtocol::CloseCodeGoingAway);
    server_socket.ws->deleteLater();
    break;
  }

  connected = false;
}

void NetworkManager::ship_server_packet(QString p_packet)
{
  switch (active_connection_type) {
  case TCP:
    server_socket.tcp->write(p_packet.toUtf8());
    break;
  case WEBSOCKETS:
    server_socket.ws->sendTextMessage(p_packet);
    break;
  }
}

void NetworkManager::handle_server_packet(const QString& p_data)
{
  QString in_data = p_data;

  if (!p_data.endsWith("%")) {
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  const QStringList packet_list = in_data.split("%", QString::SkipEmptyParts);
#else
  const QStringList packet_list = in_data.split("%", Qt::SkipEmptyParts);
#endif

  for (const QString &packet : packet_list) {
    QStringList f_contents;
    // Packet should *always* end with #
    if (packet.endsWith("#")) {
      f_contents = packet.chopped(1).split("#");
    }
    // But, if it somehow doesn't, we should still be able to handle it
    else {
      f_contents = packet.split("#");
    }
    // Empty packets are suspicious!
    if (f_contents.isEmpty()) {
      qWarning() << "WARNING: Empty packet received from server, skipping...";
      continue;
    }
    // Take the first arg as the command
    QString command = f_contents.takeFirst();
    // The rest is contents of the packet
    AOPacket *f_packet = new AOPacket(command, f_contents);
    // Ship it to the server!
    ao_app->server_packet_received(f_packet);
  }
}

void NetworkManager::start_image_streaming(QString path, QString prefix)
{
  // Reminder to change this later
  if (!prefix.isEmpty())
    path += prefix;
  streamed_path = path;
  QUrl url(path);
  QNetworkRequest request(url);

  QSslConfiguration config = request.sslConfiguration();
  config.setProtocol(QSsl::AnyProtocol);
  request.setSslConfiguration(config);
  
  request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36");
  stream->get(request);
}

void NetworkManager::image_reply_finished(QNetworkReply *reply)
{
  // Check if there was an error in the network reply
  if (reply->error() == QNetworkReply::NoError) {
      // Get the HTTP status code from the reply
      int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      qDebug() << "Status code: " << statusCode;
  
      // Handle image file download (if it's a .png file)
      if (streamed_path.endsWith(".png")) {
          QByteArray image_data = reply->readAll();
          if (streamed_image.loadFromData(image_data)) {
              streaming_successful = true;
              qDebug() << "Success loading image.";
              emit imageLoaded(streamed_image);
          } else {
              streaming_successful = false;
              qDebug() << "Failed loading image.";
          }
      } else if (streamed_path.endsWith("char.ini")) {
          // Handle .ini file download
          QByteArray char_ini_data = reply->readAll();
          QString character_folder = "base/characters/" + streamed_charname;
          QDir().mkpath(character_folder);
          QString char_ini_path = character_folder + "/char.ini";
          QFile char_ini_file(char_ini_path);
          if (char_ini_file.open(QIODevice::WriteOnly)) {
              char_ini_file.write(char_ini_data);
              char_ini_file.close();
              qDebug() << "Successfully saved char.ini for character " << streamed_charname;
          } else {
              qWarning() << "Failed to save char.ini for character " << streamed_charname;
          }
      }
  } else {
      // Handle network error
      int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      qDebug() << "Status code: " << statusCode;
      qDebug() << "Error:" << reply->errorString();
      streaming_successful = false;
      qDebug() << "Network Error while retrieving image.";
  }
  
  reply->deleteLater();
}

void NetworkManager::download_folder(const QStringList& paths) {
  for (const QString& path : paths) {
      QUrl url(path);
      QString string_url = path;
      QNetworkRequest request(url);
      QNetworkReply* reply = stream->get(request);

      QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, string_url]() {
          if (reply->error() == QNetworkReply::NoError) {
              save_folder(reply->readAll(), string_url);
          } else {
              qDebug() << "Failed to download folder: " << reply->errorString();
          }

          reply->deleteLater();
      });
  }
}


void NetworkManager::save_folder(const QByteArray& folderData, const QString& pathUrl) {
    QString localFolderPath = "base/characters/" + streamed_charname;

    QDir dir(localFolderPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QRegularExpression regex("<a href=\"(.*?)\">(.*?)<\\/a>");
    QRegularExpressionMatchIterator matches = regex.globalMatch(QString(folderData));

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        if (match.hasMatch()) {
            QString fileName = match.captured(1);
            QString fileSize = match.captured(2);

            if (fileName.isEmpty() || fileSize.isEmpty())
                continue;

            qDebug() << fileName;
            pathUrl = pathUrl + "/" + fileName;
            qDebug() << pathUrl;
            QNetworkRequest request(pathUrl);
            QNetworkReply* reply = stream->get(request);

            QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, localFolderPath, fileName]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QFile localFile(localFolderPath + "/" + fileName);
                    if (localFile.open(QIODevice::WriteOnly)) {
                        localFile.write(reply->readAll());
                        localFile.close();
                    } else {
                        qDebug() << "Error while saving: " << localFile.fileName();
                    }
                } else {
                    qDebug() << "Failed to download file: " << reply->errorString();
                }

                reply->deleteLater();
            });
        }
    }
}
