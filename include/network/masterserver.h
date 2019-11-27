#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <contrib/QtPromise>

#include "datatypes.h"

using namespace QtPromise;

namespace AttorneyOnline {

class MasterServer : public QObject
{
  Q_OBJECT

public:
  explicit MasterServer(QObject *parent = nullptr)
    : QObject(parent) {}
  virtual ~MasterServer() = default;

  virtual QPromise<void> connect(const QString &address,
                                 const uint16_t &port) = 0;
  virtual void sendKeepalive() = 0;
  virtual void sendChat(const QString &name, const QString &message) = 0;
  virtual void requestServerList();

  virtual QVector<server_type> servers();

signals:
  void chatReceived(const QString &name, const QString &message);
  void serversChanged();

};

}

#endif // MASTERSERVER_H
