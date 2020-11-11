#include "demoserver.h"
#include "lobby.h"

DemoServer::DemoServer(QObject *parent) : QObject(parent)
{
    tcp_server = new QTcpServer(this);
    connect(tcp_server, &QTcpServer::newConnection, this, &DemoServer::accept_connection);
}

void DemoServer::start_server()
{
    if (server_started) return;
    // Just use port 27088
    // This should be user configurable
    // Also, it runs into problems if multiple clients are running
    // Someone can figure out a better solution to this later, before merge
    if (!tcp_server->listen(QHostAddress::Any, 27088)) {
        qCritical() << "Could not start demo playback server...";
        qDebug() << tcp_server->errorString();
        return;
    }
    qDebug() << "Server started";
    server_started = true;
}

void DemoServer::accept_connection()
{
    if (client_sock) {
        // Client is already connected...
        qDebug() << "Multiple connections to demo server disallowed.";
        QTcpSocket* temp_socket = tcp_server->nextPendingConnection();
        connect(temp_socket, &QAbstractSocket::disconnected, temp_socket, &QObject::deleteLater);
        temp_socket->disconnectFromHost();
        return;
    }
    client_sock = tcp_server->nextPendingConnection();
    connect(client_sock, &QAbstractSocket::disconnected, this, &DemoServer::client_disconnect);
    connect(client_sock, &QAbstractSocket::readyRead, this, &DemoServer::recv_data);
    client_sock->write("decryptor#NOENCRYPT#%");
}

void DemoServer::recv_data()
{
    QString in_data = QString::fromUtf8(client_sock->readAll());

    // Copypasted from NetworkManager
    if (!in_data.endsWith("%")) {
      partial_packet = true;
      temp_packet += in_data;
      return;
    }

    else {
      if (partial_packet) {
        in_data = temp_packet + in_data;
        temp_packet = "";
        partial_packet = false;
      }
    }

    QStringList packet_list =
        in_data.split("%", QString::SplitBehavior(QString::SkipEmptyParts));

    for (QString packet : packet_list) {
        AOPacket ao_packet = AOPacket(packet);
        handle_packet(ao_packet);
    }
}

void DemoServer::handle_packet(AOPacket packet)
{
    packet.net_decode();

    QString header = packet.get_header();
    QStringList contents = packet.get_contents();

    if (header == "HI") {
        client_sock->write("ID#0#DEMOINTERNAL#0#%");
    }
    else if (header == "ID") {
        QStringList feature_list = {
                "noencryption", "yellowtext",       "prezoom",
                "flipping",     "customobjections", "fastloading",
                "deskmod",      "evidence",         "cccc_ic_support",
                "arup",         "casing_alerts",    "modcall_reason",
                "looping_sfx",  "additive",         "effects"};
        client_sock->write("PN#0#1#%");
        client_sock->write("FL#");
        client_sock->write(feature_list.join('#').toUtf8());
        client_sock->write("#%");
    }
}

void DemoServer::client_disconnect()
{
    client_sock->deleteLater();
    client_sock = nullptr;
}
