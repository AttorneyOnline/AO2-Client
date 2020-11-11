#ifndef DEMOSERVER_H
#define DEMOSERVER_H

#include "aopacket.h"

#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class DemoServer : public QObject
{
    Q_OBJECT
public:
    explicit DemoServer(QObject *parent = nullptr);

    bool server_started = false;
    int port = 27088;

private:
    void handle_packet(AOPacket packet);

    QTcpServer* tcp_server;
    QTcpSocket* client_sock = nullptr;
    bool client_connected = false;
    bool partial_packet = false;
    QString temp_packet = "";

private slots:
    void accept_connection();
    void recv_data();
    void client_disconnect();

public slots:
    void start_server();

signals:

};

#endif // DEMOSERVER_H
