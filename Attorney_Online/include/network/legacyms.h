#ifndef LEGACYMS_H
#define LEGACYMS_H

#include <QtPromise>

#include "legacysocket.h"
#include "masterserver.h"

namespace AttorneyOnline {

/*!
 * \brief An AO2-compatible master server client.
 *
 * Its functionality is very simple; however, it only allows for manual
 * polling of servers. The server makes no attempt to broadcast server list
 * changes to clients. Like the \link{LegacyClient}, this implementation
 * makes no attempt to be AO1-compatible.
 */
class LegacyMasterServer : public MasterServer
{
private:
  LegacySocket socket;

  QVector<server_type> serversList;

  void mapSignals();
public:
  explicit LegacyMasterServer(QObject *parent)
    : MasterServer(parent) {}
  QPromise<void> connect(const QString &address,
                         const uint16_t &port) override;
  void sendKeepalive() override;
  void sendChat(const QString &name, const QString &message) override;
  void requestServerList() override;

  QVector<server_type> servers() override { return serversList; }

  bool isConnected() override;
  bool isConnecting() override;
};

} // namespace AttorneyOnline


#endif // LEGACYMS_H
