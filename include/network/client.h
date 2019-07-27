#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QVector>
#include <contrib/QtPromise>

#include "datatypes.h"

using namespace QtPromise;

namespace AttorneyOnline {

class Client {
protected:
  const QString address;
  const uint16_t port;

public:
  explicit Client(const QString &address, const uint16_t &port)
    : address(address), port(port) {}
  virtual ~Client();

  virtual QPromise<void> connect() = 0;
  virtual void sendKeepalive() = 0;

  virtual QVector<char_type> characters() = 0;

  virtual QVector<area_type> rooms() = 0;
  virtual void joinRoom(int index) = 0;
  virtual void setCharacter(int charId) = 0;

  virtual void callMod(const QString &message) = 0;

  virtual void sendIC(const chat_message_type &message) = 0;
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
};

} // namespace AttorneyOnline

#endif // CLIENT_H
