#include "netwebsocketconnection.h"

#include "networkmanager.h"

#include <QNetworkRequest>
#include <QUrl>

NetWebSocketConnection::NetWebSocketConnection(NetworkManager *networkManager)
    : NetConnection(networkManager)
    , m_network_manager(networkManager)
    , m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_last_state(QAbstractSocket::UnconnectedState)
{
  connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &NetWebSocketConnection::onError);
  connect(m_socket, &QWebSocket::stateChanged, this, &NetWebSocketConnection::onStateChanged);
  connect(m_socket, &QWebSocket::textMessageReceived, this, &NetWebSocketConnection::onTextMessageReceived);
}

NetWebSocketConnection::~NetWebSocketConnection()
{
  m_socket->disconnect(this);
  disconnectFromServer();
}

bool NetWebSocketConnection::isConnected()
{
  return m_last_state == QAbstractSocket::ConnectedState;
}

void NetWebSocketConnection::connectToServer(ServerInfo &server)
{
  disconnectFromServer();

  QUrl url;
  url.setScheme("ws");
  url.setHost(server.ip);
  url.setPort(server.port);

  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::UserAgentHeader, m_network_manager->get_user_agent());

  m_socket->open(req);
}

void NetWebSocketConnection::disconnectFromServer()
{
  if (isConnected())
  {
    m_socket->close(QWebSocketProtocol::CloseCodeGoingAway);
  }
}

void NetWebSocketConnection::sendPacket(AOPacket packet)
{
  m_socket->sendTextMessage(packet.toString(true));
}

void NetWebSocketConnection::onError()
{
  Q_EMIT errorOccurred(m_socket->errorString());
}

void NetWebSocketConnection::onStateChanged(QAbstractSocket::SocketState state)
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

void NetWebSocketConnection::onTextMessageReceived(QString message)
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
