#ifndef LEGACYCLIENT_H
#define LEGACYCLIENT_H

#include <QString>
#include <QTcpSocket>
#include <QVector>
#include <contrib/QtPromise>

#include "legacysocket.h"
#include "client.h"

using namespace QtPromise;

namespace AttorneyOnline {

/*!
 * \brief An AO2-compatible client.
 *
 * This implementation of an AO2 client represents the protocol in its final
 * form. It does not include backward compatibility for unmaintained server
 * software, such as the official 1.7.5 server or serverD; as such, it is
 * intentionally missing Fantacrypt. Its only backward compatibiltiy check is
 * verifying whether or not the server supports 2.6/Case Cafe features, since
 * old versions of tsuserver are strict about the layout of the CT packet.
 */
class LegacyClient : public Client {
private:
  bool caseCafeFeatures = false;

  int pairCharId = -1;
  int pairOffset = 0;

  QVector<char_type> charsList;
  QStringList tracksList;
  QVector<evi_type> evidenceList;
  QVector<area_type> areasList;

  int currentCharId = -1;

  int curPlayers;
  int maxPlayers;

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

  LegacySocket socket;

  void mapSignals();

public:
  explicit LegacyClient(QObject *parent)
    : Client(parent) {}
  QPromise<void> connect(const QString &address,
                         const uint16_t &port,
                         const bool &probeOnly) override;
  void sendKeepalive() override;

  QVector<char_type> characters() override { return charsList; }
  char_type character() override;
  bool spectating() override { return currentCharId == -1; }

  QVector<area_type> rooms() override { return areasList; }
  void joinRoom(const QString &name) override;
  void setCharacter(int charId) override;

  void callMod(const QString &message) override;

  QPromise<void> sendIC(const chat_message_type &message) override;
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

  std::pair<int, int> playerCount() const override;

  int currentPair() const { return pairCharId; }
  void setPair(int cid) { pairCharId = cid; }

  int currentPairOffset() const { return pairOffset; }
  void setPairOffset(int offset) { pairOffset = offset; }
};

} // namespace AttorneyOnline

#endif // LEGACYCLIENT_H
