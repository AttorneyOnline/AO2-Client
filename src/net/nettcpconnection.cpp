#include "nettcpconnection.h"

NetTcpConnection::NetTcpConnection(QObject *parent)
    : NetConnection(parent)
    , m_socket(new QTcpSocket(this))
    , m_last_state(QAbstractSocket::UnconnectedState)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetTcpConnection::onErrorOccurred);
#else
  connect(m_socket, &QTcpSocket::errorOccurred, this, &NetTcpConnection::onErrorOccurred);
#endif
  connect(m_socket, &QTcpSocket::stateChanged, this, &NetTcpConnection::onStateChanged);
  connect(m_socket, &QTcpSocket::readyRead, this, &NetTcpConnection::onReadyRead);
}

NetTcpConnection::~NetTcpConnection()
{
  m_socket->disconnect(this);
  disconnectFromServer();
}

bool NetTcpConnection::isConnected()
{
  return m_last_state == QAbstractSocket::ConnectedState;
}

void NetTcpConnection::connectToServer(ServerInfo &server)
{
  disconnectedFromServer();
  m_socket->connectToHost(server.ip, server.port);
}

void NetTcpConnection::disconnectFromServer()
{
  m_socket->abort();
}

void NetTcpConnection::sendPacket(AOPacket packet)
{
  if (!isConnected())
  {
    qWarning().noquote() << QObject::tr("Cannot send packet, not connected to server");
    return;
  }
  m_socket->write(packet.toString(true).toUtf8());
}

void NetTcpConnection::onErrorOccurred()
{
  Q_EMIT errorOccurred(m_socket->errorString());
}

void NetTcpConnection::onStateChanged(QAbstractSocket::SocketState state)
{
  m_last_state = state;
  switch (state)
  {
  default:
    break;

  case QAbstractSocket::ConnectedState:
    m_cached_data.clear();
    Q_EMIT connectedToServer();
    break;

  case QAbstractSocket::UnconnectedState:
    Q_EMIT disconnectFromServer();
    break;
  }
}

void NetTcpConnection::onReadyRead()
{
  m_cached_data += QString::fromUtf8(m_socket->readAll());
  if (!m_cached_data.endsWith('%'))
  {
    return;
  }

  QStringList raw_packet_list = m_cached_data.split('%', Qt::SkipEmptyParts);
  m_cached_data.clear();
  for (QString raw_packet : raw_packet_list)
  {
    if (!raw_packet.endsWith('#'))
    {
      Q_EMIT errorOccurred(QObject::tr("Malformed packet received %1").arg(raw_packet));
      continue;
    }
    raw_packet.chop(1);

    QStringList raw_content = raw_packet.split('#');

    const QString header = raw_content.takeFirst();
    for (QString &data : raw_content)
    {
      data = AOPacket::decode(data);
    }

    Q_EMIT receivedPacket(AOPacket(header, raw_content));
  }
}
