#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <QtPromise>

#include "datatypes.h"

namespace AttorneyOnline {

class MasterServer : public QObject
{
  Q_OBJECT

public:
  explicit MasterServer(QObject *parent = nullptr)
    : QObject(parent) {}
  virtual ~MasterServer() = default;

  virtual QtPromise::QPromise<void> connect(const QString &address,
                                 const uint16_t &port) = 0;
  virtual void sendKeepalive() = 0;
  virtual void sendChat(const QString &name, const QString &message) = 0;
  virtual void requestServerList() = 0;

  virtual QVector<server_type> servers() = 0;

  virtual bool isConnected() = 0;
  virtual bool isConnecting() = 0;

signals:
  void chatReceived(const QString &name, const QString &message);
  void serversChanged();
  void connectionLost();
};

}

#endif // MASTERSERVER_H
