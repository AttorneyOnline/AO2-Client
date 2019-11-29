#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QVector>
#include <contrib/QtPromise>

#include "datatypes.h"

using namespace QtPromise;

namespace AttorneyOnline {

class Client : public QObject {
  Q_OBJECT

public:
  explicit Client(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~Client() = default;

  virtual QPromise<void> connect(const QString &address,
                                 const uint16_t &port,
                                 const bool &probeOnly = false) = 0;
  virtual void sendKeepalive() = 0;

  virtual QVector<char_type> characters() = 0;
  virtual char_type character() = 0;
  virtual bool spectating() = 0;

  virtual QVector<area_type> rooms() = 0;
  virtual void joinRoom(QString &name) = 0;
  virtual void setCharacter(int charId) = 0;

  virtual void callMod(const QString &message = QStringLiteral()) = 0;

  virtual QPromise<void> sendIC(const chat_message_type &message) = 0;
  virtual void sendOOC(const QString &oocName,
                       const QString &message) = 0;

  virtual void sendWTCE(WTCE_TYPE type) = 0;
  virtual void sendHealth(HEALTH_TYPE type, int value) = 0;

  virtual QVector<evi_type> evidence() = 0;
  virtual void addEvidence(const evi_type &evidence) = 0;
  virtual void editEvidence(const int index, const evi_type &evidence) = 0;
  virtual void removeEvidence(const int index) = 0;

  virtual QStringList tracks() = 0;
  virtual void playTrack(const QString &trackName, const QString &showname) = 0;

  virtual void announceCase(const QString &caseTitle,
                            const std::bitset<CASING_FLAGS_COUNT> &rolesNeeded) = 0;

  virtual std::pair<int, int> playerCount() const = 0;

signals:
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

#endif // CLIENT_H
