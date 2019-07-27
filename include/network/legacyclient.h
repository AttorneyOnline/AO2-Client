#ifndef LEGACYCLIENT_H
#define LEGACYCLIENT_H

#include <QString>
#include <QTcpSocket>
#include <QVector>
#include <contrib/QtPromise>

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
  Q_OBJECT
private:
  bool allowPairing = false;

  QVector<char_type> charsList;
  QStringList tracksList;
  QVector<evi_type> evidenceList;
  QVector<area_type> areasList;

  int currentCharId = -1;

  // This buffer is not limited in its storage capacity.
  // However, messages longer than 1024K are discarded.
  const int BUFFER_SOFT_LIMIT = 1024 * 1024;
  QByteArray buffer;

  QTcpSocket socket;

  void send(const QString &header, QStringList args);
  QPromise<QStringList> waitForMessage(const QString &header);
  void mapSignals();
private slots:
  void packetReceived();

public:
  QPromise<void> connect() override;
  void sendKeepalive() override;

  QVector<char_type> characters() override { return charsList; }

  QVector<area_type> rooms() override { return areasList; }
  void joinRoom(int index) override;
  void setCharacter(int charId) override;

  void callMod(const QString &message) override;

  void sendIC(const chat_message_type &message) override;
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

  void announceCase(const QString &caseTitle,
                    const std::bitset<CASING_FLAGS_COUNT> &rolesNeeded) override;

signals:
  void messageReceived(const QString &header, const QStringList &args);

  void connectProgress(int current, int max, const QString &message);

  void icReceived(const chat_message_type &message);
  void oocReceived(const QString &name, const QString &message);

  void wtceReceived(WTCE_TYPE type);
  void healthChanged(HEALTH_TYPE type, int value);
  void backgroundChanged(const QString &background);
  void trackChanged(const QString &track, const QString &showname);

  void takenCharsChanged();
  void characterChanged(int charId);
  void tracksChanged();
  void evidenceChanged();
  void areasUpdated();

  void caseCalled(const QString &message,
                  const std::bitset<CASING_FLAGS_COUNT> casingFlags);

  void kicked(const QString &message, bool banned = false);
  void modCalled(const QString &message);
};

} // namespace AttorneyOnline

#endif // LEGACYCLIENT_H
