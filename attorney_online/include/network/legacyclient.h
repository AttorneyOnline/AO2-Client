#ifndef LEGACYCLIENT_H
#define LEGACYCLIENT_H

#include <QString>
#include <QTcpSocket>
#include <QVector>
#include <QtPromise>

#include "client.h"
#include "network/socket.h"

namespace AttorneyOnline {

/*!
 * \brief An AO2-compatible client.
 *
 * This implementation of an AO2 client represents the protocol in its final
 * form. It does not include backward compatibility for unmaintained server
 * software, such as the official 1.7.5 server or serverD; as such, it is
 * intentionally missing Fantacrypt.
 */
class LegacyClient : public Client {
private:
  bool caseCafeFeatures = false;
  bool killingFeverOnlineFeatures = false;

  int pairCharId = -1;
  int pairOffset = 0;

  QVector<char_type> charsList;
  QStringList tracksList;
  QVector<evi_type> evidenceList;
  QVector<area_type> areasList;

  int currentCharId = -1;

  // tsuserver3 enjoys kicking players that do not send the keepalive
  // packet within a server-configured timeout.
  const int KEEPALIVE_INTERVAL = 60 * 1000;
  QTimer keepaliveTimer;

  // Maximum time to wait for an echo-back from the server for a sent
  // IC message. Otherwise, the promise is rejected and discarded.
  const int IC_ECHO_TIMEOUT = 5 * 1000;

  // Whether or not this client has already been kicked by the server.
  // This is used to prevent the connectionLost signal from being sent
  // twice, once by the server and again by the socket.
  bool kicked = false;

  std::unique_ptr<Socket> socket;

  void mapSignals();

public:
  explicit LegacyClient(QObject *parent)
    : Client(parent) {}
  QtPromise::QPromise<void> connect(const QString &address,
                         const uint16_t &port,
                         const bool &probeOnly,
                         const connection_type &backend) override;
  void sendKeepalive() override;

  QVector<char_type> characters() override { return charsList; }
  char_type character() override;
  bool spectating() override { return currentCharId == -1; }

  QVector<area_type> rooms() override { return areasList; }
  void joinRoom(const QString &name) override;
  void setCharacter(int charId) override;

  void callMod(const QString &message) override;

  QtPromise::QPromise<void> sendIC(const DataTypes::MSPacket &message) override;
  void sendOOC(const QString &oocName,
               const QString &message) override;

  void sendWTCE(WTCE_TYPE type) override;
  void sendHealth(HEALTH_TYPE type, int value) override;

  QVector<evi_type> evidence() override { return evidenceList; }
  void addEvidence(const evi_type &evidence) override;
  void editEvidence(const int index, const evi_type &evidence) override;
  void removeEvidence(const int index) override;

  QStringList tracks() override { return tracksList; }
  void playTrack(const QString &trackName, const QString &showname) override;

  int currentPair() const { return pairCharId; }
  void setPair(int cid) { pairCharId = cid; }

  int currentPairOffset() const { return pairOffset; }
  void setPairOffset(int offset) { pairOffset = offset; }
};

} // namespace AttorneyOnline

#endif // LEGACYCLIENT_H
