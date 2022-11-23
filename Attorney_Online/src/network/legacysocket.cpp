#include "network/legacysocket.h"

#include <QRegularExpression>
#include <QStringBuilder>

namespace AttorneyOnline {

/*!
 * Composes and sends a message to the server.
 *
 * \param header  the header of the message
 * \param args  a list of arguments
 */
void LegacySocket::packetReceived(QByteArray message)
{
    buffer.append(message);

  if (buffer.size() >= BUFFER_SOFT_LIMIT) {
    qWarning() << QStringLiteral("Buffer reached over %1 bytes (%2 bytes)! "
                                 "Discarding buffer.")
                      .arg(BUFFER_SOFT_LIMIT)
                      .arg(buffer.size());
    buffer.clear();
    return;
  }

  int end;
  while ((end = buffer.indexOf('%')) != -1) {
    if (end == 0) {
      // Special case of a garbage message - kill with fire
      buffer = buffer.mid(end + 1);
      continue;
    }

    QByteArray msg = buffer.left(end - 1); // Ignore trailing "%"
    QStringList args = QString::fromUtf8(msg)
                           .split("#")
                           .replaceInStrings("<num>", "#")
                           .replaceInStrings("<percent>", "%")
                           .replaceInStrings("<dollar>", "$")
                           .replaceInStrings("<and>", "&");
    QString header = args.takeFirst();

    // Sometimes packets don't come with a # at the end, which is extremely
    // annoying. This is why we cannot always trim the buffer by two
    // characters.
    if (msg[msg.size() - 1] == '#') args.removeLast();

    QStringList debugArgs = args;
    debugArgs.replaceInStrings(QRegularExpression("(.{60}).+(.{60})"),
                               "\\1...\\2");
    qDebug() << header << debugArgs;

    emit messageReceived(header, args);

    // (Unfortunately QByteArray is not a circular buffer. You wish it were,
    // though.)
    buffer = buffer.mid(end + 1);
  }
}

QPromise<void> LegacySocket::connect(const QString &address,
                                     const uint16_t &port,
                                     const connection_type &type)
{
  active_connection_type = type;
  switch (active_connection_type) {
  case TCP: {
    qInfo() << "using TCP backend";
    QObject::connect(socket.tcp, &QIODevice::readyRead, this, [this] {
        packetReceived(socket.tcp->readAll());
      });

    // (QTcpSocket::error is overloaded, so we have to select the right one)
    auto errorFunc =
        static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(
            &QTcpSocket::errorOccurred);

    auto promise =
        QtPromise::connect(socket.tcp, &QTcpSocket::connected, errorFunc)
            .then([&] {
              QObject::connect(socket.tcp, &QTcpSocket::disconnected, this,
                               &LegacySocket::connectionLost);
            })
            .timeout(TIMEOUT_MILLISECS)
            .fail([&](const QPromiseTimeoutException &) {
              socket.tcp->disconnectFromHost();
            });

    // Connect TCP socket, bringing the promise chain above into motion.
    socket.tcp->connectToHost(address, port);
    return promise;
  }
  case WEBSOCKETS:
    qInfo() << "using Websocket backend";
    QObject::connect(socket.ws, &QWebSocket::binaryMessageReceived, this,
                     &LegacySocket::packetReceived);

    // (QTcpSocket::error is overloaded, so we have to select the right one)
    auto errorFunc =
        static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(
            &QWebSocket::error);

    auto promise =
        QtPromise::connect(socket.ws, &QWebSocket::connected, errorFunc)
            .then([&] {
              QObject::connect(socket.ws, &QWebSocket::disconnected, this,
                               &LegacySocket::connectionLost);
            })
            .timeout(TIMEOUT_MILLISECS)
            .fail(
                [&](const QPromiseTimeoutException &) { socket.ws->close(); });

    QUrl url;
    url.setScheme("ws");
    url.setHost(address);
    url.setPort(port);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "FOO");
    socket.ws->open(req);
    return promise;
  }
}

void LegacySocket::send(const QString &header, QStringList args)
{
  args.replaceInStrings("#", "<num>")
      .replaceInStrings("%", "<percent>")
      .replaceInStrings("$", "<dollar>")
      .replaceInStrings("&", "<and>");

  auto bytes = (header % "#" % args.join('#') % "#%").toUtf8();
  qDebug().noquote() << bytes;
  switch (active_connection_type) {
  case TCP:
    socket.tcp->write(bytes, bytes.length());
    break;
  case WEBSOCKETS:
    socket.ws->sendBinaryMessage(bytes);
  }
}

/*!
 * Waits until a message with the specified header is received.
 *
 * \param header  the header to wait for
 * \return a list of parameters sent with the message
 */
QPromise<QStringList> LegacySocket::waitForMessage(const QString &header)
{
  qDebug().noquote() << "Waiting for" << header;

  return QPromise<QStringList>(
      [&](const QPromiseResolve<QStringList> &resolve) {
        std::shared_ptr<QMetaObject::Connection> connection =
            std::make_shared<QMetaObject::Connection>();

        *connection = QObject::connect(
            this, &LegacySocket::messageReceived,
            [=](const QString &recvHeader, const QStringList &args) {
              if (recvHeader == header) {
                qDebug().noquote() << "Resolving with" << recvHeader;
                QObject::disconnect(*connection);
                resolve(args);
              }
            });
      });
}

bool LegacySocket::isConnected()
{
  switch (active_connection_type) {
  case TCP:
    return socket.tcp->state() == QAbstractSocket::ConnectedState;
    break;
  case WEBSOCKETS:
    return socket.ws->state() == QAbstractSocket::ConnectedState;
      break;
  }
  return false;
}

bool LegacySocket::isConnecting()
{
  switch (active_connection_type) {
  case TCP:
    return socket.tcp->state() == QAbstractSocket::ConnectingState;
    break;
  case WEBSOCKETS:
    return socket.ws->state() == QAbstractSocket::ConnectingState;
    break;
  }
  return false;
}

} // namespace AttorneyOnline
