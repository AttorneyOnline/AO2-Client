#include "network/legacyms.h"

#include "network/hdid.h"

namespace AttorneyOnline {

void LegacyMasterServer::mapSignals()
{
  QObject::connect(&socket, &LegacySocket::messageReceived, this,
                   [&](const QString &header, const QStringList &args) {
    if (header == "ALL")
    {
      serversList.clear();

      for (const QString &entry : args)
      {
        server_type server;
        QStringList contents = entry.split("&");

        if (contents.size() < 4)
        {
          qWarning() << "Malformed ALL packet entry";
          continue;
        }

        server.name = contents[0];
        server.desc = contents[1];
        server.ip = contents[2];
        server.port = contents[3].toInt();

        serversList.append(std::move(server));
      }

      emit serversChanged();
    }
    else if (header == "CT")
    {
      if (args.size() == 1)
      {
        emit chatReceived(QLatin1String(), args[0]);
      }
      else
      {
        emit chatReceived(args[0], args[1]);
      }
    }
    else if (header == "AO2CHECK")
    {
      socket.send("ID", { "AO2", QCoreApplication::applicationVersion() });
      socket.send("HI", { hdid() });

      // TODO: Meh, who cares about version checking right now.
    }
  });

  QObject::connect(&socket, &LegacySocket::connectionLost,
                   this, &MasterServer::connectionLost);
}

/*!
 * Establishes a TCP connection to the master server. No handshake is
 * required by the MS to ask for servers.
 *
 * \return a promise that is resolved when the connection is successful
 */
QPromise<void> LegacyMasterServer::connect(const QString &address,
                                           const uint16_t &port)
{
  if (socket.isConnected() || socket.isConnecting())
  {
    qWarning() << "connect call while still connecting to master server!";
    return QPromise<void>::resolve();
  }

  return socket.connect(address, port)
      .then([&](void)
  {
    mapSignals();
  });
}

// XXX: currently unused
void LegacyMasterServer::sendKeepalive()
{
  socket.send("CHECK");
}

/*!
 * Sends a chat message to the master server.
 * \param name     name to chat with
 * \param message  plain-text message to be sent
 */
void LegacyMasterServer::sendChat(const QString &name, const QString &message)
{
  socket.send("CT", { name, message });
}

/*!
 * Requests that the master server provide the client with an updated server
 * list.
 */
void LegacyMasterServer::requestServerList()
{
  socket.send("ALL");
}

bool LegacyMasterServer::isConnected()
{
  return socket.isConnected();
}

bool LegacyMasterServer::isConnecting()
{
  return socket.isConnecting();
}

} // namespace AttorneyOnline
