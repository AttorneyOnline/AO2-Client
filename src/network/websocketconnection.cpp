#include "websocketconnection.h"

#include "aoapplication.h"

#include <QNetworkRequest>
#include <QUrl>

WebSocketConnection::WebSocketConnection(AOApplication *ao_app, QObject *parent)
    : QObject(parent)
    , ao_app(ao_app)
    , m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_last_state(QAbstractSocket::UnconnectedState)
{
  connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WebSocketConnection::onError);
  connect(m_socket, &QWebSocket::stateChanged, this, &WebSocketConnection::onStateChanged);
  connect(m_socket, &QWebSocket::textMessageReceived, this, &WebSocketConnection::onTextMessageReceived);
}

WebSocketConnection::~WebSocketConnection()
{
  m_socket->disconnect(this);
  disconnectFromServer();
}

bool WebSocketConnection::isConnected()
{
  return m_last_state == QAbstractSocket::ConnectedState;
}

void WebSocketConnection::connectToServer(const ServerInfo &server)
{
  disconnectFromServer();

  QUrl url;
  url.setScheme("ws");
  url.setHost(server.address);
  url.setPort(server.port);

  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("AttorneyOnline/%1 (Desktop)").arg(ao_app->get_version_string()));

  m_socket->open(req);
}

void WebSocketConnection::disconnectFromServer()
{
  if (isConnected())
  {
    m_socket->close(QWebSocketProtocol::CloseCodeGoingAway);
  }
}

void WebSocketConnection::sendPacket(AOPacket packet)
{
  m_socket->sendTextMessage(packet.toString(true));
}

void WebSocketConnection::onError()
{
  Q_EMIT errorOccurred(m_socket->errorString());
}

void WebSocketConnection::onStateChanged(QAbstractSocket::SocketState state)
{
  m_last_state = state;
  switch (state)
  {
  default:
    break;

  case QAbstractSocket::ConnectedState:
    Q_EMIT connectedToServer();
    break;

  case QAbstractSocket::UnconnectedState:
    Q_EMIT disconnectedFromServer();
    break;
  }
}

void WebSocketConnection::onTextMessageReceived(QString message)
{
  if (!message.endsWith("#%"))
  {
    return;
  }
  message.chop(2);

  QStringList raw_content = message.split('#');
  const QString header = raw_content.takeFirst();
  for (QString &data : raw_content)
  {
    data = AOPacket::decode(data);
  }

  Q_EMIT receivedPacket(AOPacket(header, raw_content));
}
