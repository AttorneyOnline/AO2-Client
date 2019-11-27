#ifndef LEGACYSOCKET_H
#define LEGACYSOCKET_H

#include <QTcpSocket>
#include <contrib/QtPromise>

using namespace QtPromise;

namespace AttorneyOnline {

class LegacySocket : public QObject {
  Q_OBJECT

private:
  QTcpSocket socket;

  // This buffer is not limited in its storage capacity.
  // However, messages longer than 1024K are discarded.
  const int BUFFER_SOFT_LIMIT = 1024 * 1024;
  QByteArray buffer;

private slots:
  void packetReceived();

public:
  explicit LegacySocket(QObject *parent = nullptr)
    : QObject(parent) {}

  QPromise<void> connect(const QString &address, const uint16_t &port);
  void send(const QString &header, QStringList args = {});

  QPromise<QStringList> waitForMessage(const QString &header);

signals:
  void messageReceived(const QString &header, const QStringList &args);
};

} // namespace AttorneyOnline

#endif // LEGACYSOCKET_H
