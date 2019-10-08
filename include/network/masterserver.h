#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <contrib/QtPromise>

using namespace QtPromise;

namespace AttorneyOnline {

class MasterServer : public QObject
{
  Q_OBJECT

protected:
  const QString address;
  const uint16_t port;

public:
  explicit MasterServer(QObject *parent,
                        const QString &address, const uint16_t &port)
    : QObject(parent), address(address), port(port) {}
  virtual ~MasterServer() = default;

  virtual QPromise<void> connect() = 0;
  virtual void sendKeepalive() = 0;
  virtual void sendChat(const QString &name, const QString &message) = 0;
  virtual void requestServerList();

signals:
  void messageReceived(const QString &header, const QStringList &args);
  void chatReceived(const QString &name, const QString &message);
  void serversChanged();

};

}

#endif // MASTERSERVER_H
