#ifndef LEGACYSOCKET_H
#define LEGACYSOCKET_H


#include <QTcpSocket>
#include <QtPromise>

#include "socket.h"

namespace AttorneyOnline {

class LegacySocket : public Socket {
  Q_OBJECT

private:
  QTcpSocket socket;

  // This buffer is not limited in its storage capacity.
  // However, messages longer than 1024K are discarded.
  const int BUFFER_SOFT_LIMIT = 1024 * 1024;
  QByteArray buffer;

  const int TIMEOUT_MILLISECS = 4000;

private slots:
  void packetReceived();

public:
  explicit LegacySocket(QObject *parent = nullptr)
      : Socket(parent) {}

  QtPromise::QPromise<void> connect(const QString &address, const uint16_t &port) override;
  void send(const QString &header, QStringList args = {}) override;

  QtPromise::QPromise<QStringList> waitForMessage(const QString &header) override;

  bool isConnected() override;
  bool isConnecting() override;
};

} // namespace AttorneyOnline

#endif // LEGACYSOCKET_H
