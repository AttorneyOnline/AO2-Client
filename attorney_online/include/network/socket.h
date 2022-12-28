#ifndef SOCKET_H
#define SOCKET_H

#include <QtPromise>

namespace AttorneyOnline {

class Socket : public QObject {
  Q_OBJECT

public:
  explicit Socket(QObject *parent = nullptr)
    : QObject(parent) {}
  virtual ~Socket() = default;

  virtual QtPromise::QPromise<void> connect(const QString &address, const uint16_t &port) = 0;
  virtual void send(const QString &header, QStringList args = {}) = 0;

  virtual QtPromise::QPromise<QStringList> waitForMessage(const QString &header) = 0;

  virtual bool isConnected() = 0;
  virtual bool isConnecting() = 0;

signals:
  void messageReceived(const QString &header, const QStringList &args);
  void connectionLost();
};

} // namespace AttorneyOnline

#endif // SOCKET_H
