#ifndef LEGACYSOCKET_WS_H
#define LEGACYSOCKET_WS_H


#include <QtWebSockets/QWebSocket>
#include <QtPromise>

#include "socket.h"

using namespace QtPromise;

namespace AttorneyOnline {

class LegacySocket_WS: public Socket {
  Q_OBJECT

private:
  QWebSocket socket;

  // This buffer is not limited in its storage capacity.
  // However, messages longer than 1024K are discarded.
  const int BUFFER_SOFT_LIMIT = 1024 * 1024;
  QByteArray buffer;

  const int TIMEOUT_MILLISECS = 4000;

private slots:
  void packetReceived(QString message);

public:
  explicit LegacySocket_WS(QObject *parent = nullptr)
      : Socket(parent) {}

  QPromise<void> connect(const QString &address, const uint16_t &port) override;;
  void send(const QString &header, QStringList args = {}) override;;

  QPromise<QStringList> waitForMessage(const QString &header) override;;

  bool isConnected() override;;
  bool isConnecting() override;;
};

} // namespace AttorneyOnline

#endif // LEGACYSOCKET_H
