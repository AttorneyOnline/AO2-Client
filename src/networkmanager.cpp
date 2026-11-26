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
#include <QDesktopServices>
#include <QPushButton>

NetworkManager::NetworkManager(AOApplication *parent) : QObject(parent)
{
  ao_app = parent;

  http = new QNetworkAccessManager(this);
  heartbeat_timer = new QTimer(this);
  stream = new QNetworkAccessManager(this);
  download = new QNetworkAccessManager(this);
  
  QString master_config =
      Options::getInstance().alternativeMasterserver();
  if (!master_config.isEmpty() && QUrl(master_config).scheme().startsWith("http")) {
    qInfo() << "using alternate master server" << master_config;
    ms_baseurl = master_config;
  }

  connect(heartbeat_timer, &QTimer::timeout, this, &NetworkManager::send_heartbeat);
  heartbeat_timer->start(heartbeat_interval);

  liveness_timer = new QTimer(this);
  liveness_timer->setInterval(kHeartbeatMs);
  connect(liveness_timer, &QTimer::timeout, this,
          &NetworkManager::send_liveness_probe);

  liveness_deadline = new QTimer(this);
  liveness_deadline->setSingleShot(true);
  liveness_deadline->setInterval(kProbeTimeoutMs);
  connect(liveness_deadline, &QTimer::timeout, this,
          &NetworkManager::on_liveness_deadline);

  // The AO handshake is complete once the server sends FL. Starting the
  // monitor there avoids probing before the connection is usable.
  connect(this, &NetworkManager::server_ready, this,
          &NetworkManager::start_liveness_monitor);

  // After a suspend or a long freeze the monotonic timers did not advance, so
  // do not wait for the next scheduled tick to discover a dead link.
  connect(qApp, &QGuiApplication::applicationStateChanged, this,
          [this](Qt::ApplicationState state) {
            if (state != Qt::ApplicationActive || !connected)
              return;
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (last_inbound_wall_ms > 0 &&
                now - last_inbound_wall_ms > kHeartbeatMs + kProbeTimeoutMs) {
              qInfo() << "application resumed after a gap, probing connection";
              probe_connection(true);
            }
          });

  connect(stream, &QNetworkAccessManager::finished, this, &NetworkManager::image_reply_finished);
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
    server.players = entry["players"].toInt(-1);
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
  QUrl url;
  if (document_type == MSDocumentType::ClientVersion) {
    url = QUrl("https://file.garden/YEp8caMz-xKjrHSj/Attorney%20Online%20Golden%20Releases/current_version.json");
  } else {
    url = QUrl(ms_baseurl + endpoint);
  }

  QNetworkRequest req(url);
  if (document_type != MSDocumentType::ClientVersion) {
    req.setRawHeader("User-Agent", get_user_agent().toUtf8());
  } else {
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36");
  }
  
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
  if (!ao_app->ensureValidTheme()) {
    ao_app->promptMissingThemes();
    Q_EMIT server_connected(false);
    return;
  }

  disconnect_from_server();

  // A new attempt starts a fresh notification scope. disconnect_from_server()
  // set intentional_disconnect so any late signal from the previous socket is
  // ignored.
  intentional_disconnect = false;
  disconnect_notice_shown = false;
  send_failure_reported = false;
  reset_connection_state();

  qInfo().nospace().noquote() << "connecting to " << p_server.ip << ":"
                              << p_server.port;
  last_server_chosen = p_server;

  switch (p_server.socket_type) {
  default:
    p_server.socket_type = TCP;
    [[fallthrough]];
  case TCP: {
    qInfo() << "using TCP backend";
    QTcpSocket *socket = new QTcpSocket(this);
    server_socket.tcp = socket;
    active_connection_type = TCP;

    connect(socket, &QAbstractSocket::connected, this, [this, socket] {
      if (socket != server_socket.tcp)
        return;
      qDebug() << "established connection to server";
      established_connection = true;
      Q_EMIT server_connected(established_connection);
    });
    connect(socket, &QIODevice::readyRead, this, [this, socket] {
      if (socket != server_socket.tcp)
        return;
      tcp_buffer += socket->readAll();
      if (tcp_buffer.size() > kMaxTcpBufferBytes) {
        qWarning() << "dropping oversized TCP buffer without packet terminator";
        tcp_buffer.clear();
        return;
      }
      // Decode only complete packets so a multi-byte UTF-8 character split
      // across reads is not corrupted.
      int terminator = tcp_buffer.indexOf('%');
      while (terminator != -1) {
        const QByteArray packet = tcp_buffer.left(terminator + 1);
        tcp_buffer.remove(0, terminator + 1);
        handle_server_packet(QString::fromUtf8(packet));
        terminator = tcp_buffer.indexOf('%');
      }
    });
    connect(socket, &QAbstractSocket::disconnected, this, [this, socket] {
      if (socket != server_socket.tcp)
        return;
      on_transport_disconnected();
    });
    connect(socket, &QAbstractSocket::errorOccurred, this,
            [this, socket](QAbstractSocket::SocketError) {
              if (socket != server_socket.tcp)
                return;
              const QString error = socket->errorString();
              qCritical() << "TCP socket error:" << error;
              if (!established_connection) {
                established_connection = false;
                Q_EMIT server_connected(false);
              }
              handle_connection_lost(tr("Connection error: %1").arg(error));
            });

    socket->connectToHost(p_server.ip, p_server.port);
    break;
  }
  case WEBSOCKETS: {
    qInfo() << "using WebSockets backend";
    QWebSocket *socket = new QWebSocket(
        QString(), QWebSocketProtocol::VersionLatest, this);
    server_socket.ws = socket;
    active_connection_type = WEBSOCKETS;

    connect(socket, &QWebSocket::connected, this, [this, socket] {
      if (socket != server_socket.ws)
        return;
      qDebug() << "established connection to server";
      established_connection = true;
      Q_EMIT server_connected(established_connection);
    });
    connect(socket, &QWebSocket::textMessageReceived, this,
            &NetworkManager::handle_server_packet);
    connect(socket, &QWebSocket::pong, this,
            [this, socket](quint64, const QByteArray &) {
              if (socket != server_socket.ws)
                return;
              last_inbound_wall_ms = QDateTime::currentMSecsSinceEpoch();
              on_check_received();
            });
    connect(socket, &QWebSocket::disconnected, this, [this, socket] {
      if (socket != server_socket.ws)
        return;
      on_transport_disconnected();
    });
    connect(socket, &QWebSocket::errorOccurred, this,
            [this, socket](QAbstractSocket::SocketError) {
              if (socket != server_socket.ws)
                return;
              const QString error = socket->errorString();
              qCritical() << "WebSockets error:" << error;
              if (!established_connection) {
                established_connection = false;
                Q_EMIT server_connected(false);
              }
              handle_connection_lost(tr("Connection error: %1").arg(error));
            });

    QUrl url;
    url.setScheme("ws");
    url.setHost(p_server.ip);
    url.setPort(p_server.port);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, get_user_agent());
    socket->open(req);
    break;
  }
  }

  connected = true;
}

void NetworkManager::join_to_server()
{
    if (!ao_app->ensureValidTheme()) {
      ao_app->promptMissingThemes();
      return;
    }

    // make sure to clear our subtheme so previously joined server doesn't affect it
    Options::getInstance().setServerSubTheme(QString());
    ship_server_packet(AOPacket("askchaa").to_string());
}

bool NetworkManager::is_connection_active() const
{
  switch (active_connection_type) {
  case TCP:
    return server_socket.tcp != nullptr &&
           server_socket.tcp->state() == QAbstractSocket::ConnectedState;
  case WEBSOCKETS:
    return server_socket.ws != nullptr &&
           server_socket.ws->state() == QAbstractSocket::ConnectedState;
  }
  return false;
}

void NetworkManager::reset_connection_state()
{
  stop_liveness_monitor();
  partial_packet = false;
  temp_packet.clear();
  tcp_buffer.clear();
  probe_outstanding = false;
  server_supports_check = false;
  last_inbound_wall_ms = 0;
  last_latency_ms = -1;
  send_failure_reported = false;
  connected = false;
  established_connection = false;
}

void NetworkManager::start_liveness_monitor()
{
  if (!connected || !is_connection_active())
    return;

  // A (re)handshake starts without knowing whether this server answers CH.
  server_supports_check = false;
  probe_outstanding = false;
  liveness_deadline->stop();
  last_inbound_wall_ms = QDateTime::currentMSecsSinceEpoch();
  liveness_timer->start(kHeartbeatMs);
  qInfo() << "connection liveness monitor started";
}

void NetworkManager::stop_liveness_monitor()
{
  if (liveness_timer)
    liveness_timer->stop();
  if (liveness_deadline)
    liveness_deadline->stop();
  probe_outstanding = false;
}

void NetworkManager::probe_connection(bool force)
{
  if (!connected || !is_connection_active())
    return;

  if (probe_outstanding && !force)
    return;

  const bool use_ws_ping = active_connection_type == WEBSOCKETS;
  if (!use_ws_ping && !server_supports_check) {
    // Still send CH so servers that use it as a keepalive (e.g. tsuserver3
    // forks) do not drop us, but do not arm a deadline until we have seen a
    // CHECK reply. The bundled demo server never implements CH.
    ship_server_packet(
        AOPacket("CH", {QString::number(character_id)}).to_string());
    return;
  }

  probe_outstanding = true;
  liveness_ping_timer.start();
  liveness_deadline->start(kProbeTimeoutMs);

  ship_server_packet(
      AOPacket("CH", {QString::number(character_id)}).to_string());

  if (use_ws_ping) {
    server_socket.ws->ping(
        QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
  }
}

void NetworkManager::send_liveness_probe()
{
  probe_connection(false);
}

void NetworkManager::on_liveness_deadline()
{
  if (!probe_outstanding)
    return;

  qWarning() << "connection liveness probe timed out";
  handle_connection_lost(tr("Connection timed out."));
}

void NetworkManager::on_check_received()
{
  server_supports_check = true;
  if (!probe_outstanding)
    return;

  last_latency_ms = liveness_ping_timer.elapsed();
  probe_outstanding = false;
  liveness_deadline->stop();
}

void NetworkManager::on_transport_disconnected()
{
  const bool should_notify = established_connection &&
                             !intentional_disconnect &&
                             !disconnect_notice_shown;

  stop_liveness_monitor();
  connected = false;
  established_connection = false;
  partial_packet = false;
  temp_packet.clear();
  tcp_buffer.clear();

  if (active_connection_type == TCP && server_socket.tcp) {
    server_socket.tcp->deleteLater();
    server_socket.tcp = nullptr;
  } else if (active_connection_type == WEBSOCKETS && server_socket.ws) {
    server_socket.ws->deleteLater();
    server_socket.ws = nullptr;
  }

  if (should_notify) {
    disconnect_notice_shown = true;
    Q_EMIT server_connected(false);
    Q_EMIT connectionLost(tr("Disconnected from server."));
  }
}

void NetworkManager::handle_connection_lost(const QString &reason)
{
  if (disconnect_notice_shown) {
    reset_connection_state();
    return;
  }

  const bool should_notify =
      established_connection && !intentional_disconnect;
  disconnect_notice_shown = true;

  stop_liveness_monitor();
  connected = false;
  established_connection = false;
  partial_packet = false;
  temp_packet.clear();
  tcp_buffer.clear();

  if (active_connection_type == TCP && server_socket.tcp) {
    QTcpSocket *socket = server_socket.tcp;
    server_socket.tcp = nullptr;
    socket->abort();
    socket->deleteLater();
  } else if (active_connection_type == WEBSOCKETS && server_socket.ws) {
    QWebSocket *socket = server_socket.ws;
    server_socket.ws = nullptr;
    socket->abort();
    socket->deleteLater();
  }

  if (should_notify) {
    qWarning().noquote() << "connection lost:" << reason;
    Q_EMIT server_connected(false);
    Q_EMIT connectionLost(reason);
  } else {
    qInfo().noquote() << "connection closed:" << reason;
  }
}

void NetworkManager::disconnect_from_server()
{
  intentional_disconnect = true;
  stop_liveness_monitor();
  connected = false;
  established_connection = false;
  partial_packet = false;
  temp_packet.clear();
  tcp_buffer.clear();

  switch (active_connection_type) {
  case TCP:
    if (server_socket.tcp) {
      QTcpSocket *socket = server_socket.tcp;
      server_socket.tcp = nullptr;
      socket->close();
      socket->deleteLater();
    }
    break;
  case WEBSOCKETS:
    if (server_socket.ws) {
      QWebSocket *socket = server_socket.ws;
      server_socket.ws = nullptr;
      socket->close(QWebSocketProtocol::CloseCodeGoingAway);
      socket->deleteLater();
    }
    break;
  }
}

void NetworkManager::ship_server_packet(QString p_packet)
{
  if (!is_connection_active()) {
    qWarning() << "dropping packet because the socket is not connected";
    if (!send_failure_reported) {
      send_failure_reported = true;
      Q_EMIT packetSendFailed();
    }
    return;
  }

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
  last_inbound_wall_ms = QDateTime::currentMSecsSinceEpoch();

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

    // Liveness bookkeeping must happen before the packet is dispatched,
    // because the courtroom may not exist yet during the handshake.
    if (command == "CHECK")
      on_check_received();

    // The rest is contents of the packet
    AOPacket *f_packet = new AOPacket(command, f_contents);
    // Ship it to the server!
    ao_app->server_packet_received(f_packet);
  }
}

void NetworkManager::start_image_streaming(QString path, QString prefix)
{
  QStringList prefixes = { ".png", ".webp", ".apng" };
  if (!prefixes.contains(prefix)) {
    path += prefix;
    streamed_path = path;
    QUrl url(path);
    QNetworkRequest request(url);
    stream->get(request);
  } else {
    foreach (const QString &imagePrefix, prefixes) {
      QString fullImagePath = path + imagePrefix;
      streamed_path = fullImagePath;
      
      QUrl url(fullImagePath);
      QNetworkRequest request(url);

      QSslConfiguration config = request.sslConfiguration();
      // config.setProtocol(QSsl::AnyProtocol);
      request.setSslConfiguration(config);
      
      request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36");
      stream->get(request);

      // Sends a lowercase network request. IDK why I deleted this before, but w/e
      QString path_lower = fullImagePath.toLower();
      QUrl url_lower(path_lower);
      QNetworkRequest request_lower(url_lower);
      stream->get(request_lower);
    }
  }
}

void NetworkManager::image_reply_finished(QNetworkReply *reply)
{
  // Check if there was an error in the network reply
  if (reply->error() == QNetworkReply::NoError) {
      // Get the HTTP status code from the reply
      int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      qDebug() << "No error, Status code: " << statusCode;

      if (streamed_path.endsWith("char.ini")) {
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
      } else { // Handle image file download
        QByteArray image_data = reply->readAll();
        if (streamed_image.loadFromData(image_data)) {
            streaming_successful = true;
            qDebug() << "Success loading image.";
            emit imageLoaded(streamed_image);
        } else {
            streaming_successful = false;
            qDebug() << "Failed loading image.";
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
      QString local_folder_path = "base/characters/" + streamed_charname;
      QNetworkRequest request(url);
      QNetworkReply* reply = download->get(request);

      QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, string_url, local_folder_path]() {
          if (reply->error() == QNetworkReply::NoError) {
              save_folder(reply->readAll(), string_url, local_folder_path);
              reply->deleteLater();
          } else {
              qDebug() << "Failed to download folder: " << reply->errorString();
              reply->deleteLater();
          }

          reply->deleteLater();
      });
  }
}


void NetworkManager::save_folder(const QByteArray& folderData, const QString& pathUrl, const QString& localFolderPath_r) {
    QString localFolderPath = localFolderPath_r;

    QString sanitizedFolderName = QUrl::fromPercentEncoding(localFolderPath.toUtf8());
    QDir dir(sanitizedFolderName);
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

          if (fileName.isEmpty() || fileSize.isEmpty() || fileName.startsWith("?"))
              continue;
        
          QString subfolderPath = localFolderPath + "/" + fileName;
          QString onlineSubfolderLookup = pathUrl + "/" + fileName;

          if (fileName.endsWith("/") && fileName != ("/base/characters/") && !fileName.startsWith("/") && !fileName.startsWith("..")) {
                qDebug() << "Subfolder found! Downloading directory: " << onlineSubfolderLookup;
                qDebug() << "Subfolder path: " << subfolderPath;

                QString sanitizedSubfolderName = QUrl::fromPercentEncoding(subfolderPath.toUtf8());
                QDir dir2(sanitizedSubfolderName);
                if (!dir2.exists() && dir2 != ("base/characters/" + streamed_charname)) {
                    dir2.mkpath(".");
                }
            
                // Recursively call the function to save the subfolder
                QNetworkRequest request(onlineSubfolderLookup);
                QNetworkReply* reply = download->get(request);
                QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, subfolderPath, onlineSubfolderLookup]() {
                    if (reply->error() == QNetworkReply::NoError) {
                        save_folder(reply->readAll(), onlineSubfolderLookup, subfolderPath); // Recursive call
                    } else {
                        qDebug() << "Failed to download folder: " << reply->errorString();
                    }

                    reply->deleteLater();
                });
          } else {
              // It's a file, not a directory
              qDebug() << "Downloading file: " << fileName;
              QString finalPathUrl = pathUrl + "/" + fileName;
              QNetworkRequest request(finalPathUrl);
              QNetworkReply* reply = download->get(request);

              QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, localFolderPath, fileName]() {
                  if (reply->error() == QNetworkReply::NoError) {
                      QString fileName_decoded = QUrl::fromPercentEncoding(fileName.toUtf8());
                      QString localFolderPath_decoded = QUrl::fromPercentEncoding(localFolderPath.toUtf8());
                      QFile localFile(localFolderPath_decoded + "/" + fileName_decoded);
                      if (localFile.exists()) {
                          qDebug() << "File already exists: " << localFile.fileName();
                      } else {
                          if (localFile.open(QIODevice::WriteOnly)) {
                              localFile.write(reply->readAll());
                              localFile.close();
                          } else {
                              qDebug() << "Error while saving: " << localFile.fileName();
                          }
                      }
                  } else {
                      qDebug() << "Failed to download file: " << reply->errorString();
                  }
  
                  reply->deleteLater();
              });
          }
      }
  }
}
