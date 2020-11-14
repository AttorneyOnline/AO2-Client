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

    QString p_path = QFileDialog::getOpenFileName(nullptr, tr("Load Demo"), "logs/", tr("Demo Files (*.demo)"));
    if (p_path.isEmpty())
      return;

    load_demo(p_path);
    // Demo starts with a newline for some reason
    demo_data.dequeue();
    sc_packet = demo_data.dequeue();
    AOPacket sc(sc_packet);
    num_chars = sc.get_contents().length();
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
        AOPacket ao_packet(packet);
        handle_packet(ao_packet);
    }
}

void DemoServer::handle_packet(AOPacket packet)
{
    packet.net_decode();

    // This code is literally a barebones AO server
    // It is wise to do it this way, because I can
    // avoid touching any of this disgusting shit
    // related to hardcoding this stuff in.

    // Also, at some point, I will make akashit
    // into a shared library.

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
    else if (header == "askchaa") {
        client_sock->write("SI#");
        client_sock->write(QString::number(num_chars).toUtf8());
        client_sock->write("#0#1#%");
    }
    else if (header == "RC") {
        client_sock->write(sc_packet.toUtf8());
    }
    else if (header == "RM") {
        // Empty music list crashes AO2 lol
        client_sock->write("SM#~stop.mp3#%");
    }
    else if (header == "RD") {
        client_sock->write("DONE#%");
    }
    else if (header == "CC") {
        client_sock->write("PV#0#CID#0#%");
    }
    else if (header == "CT") {
        if (contents[1].startsWith("/play"))
            playback();
    }
}

void DemoServer::load_demo(QString filename)
{
    QFile demo_file(filename);
    demo_file.open(QIODevice::ReadOnly);
    if (!demo_file.isOpen())
        return;

    QTextStream demo_stream(&demo_file);
    QString line = demo_stream.readLine();
    while (!line.isNull()) {
        demo_data.enqueue(line);
        line = demo_stream.readLine();
    }

    //playback();
}

void DemoServer::playback()
{
    QString current_packet = demo_data.dequeue();
    while (!current_packet.startsWith("wait") && !demo_data.isEmpty()) {
        client_sock->write(current_packet.toUtf8());
        current_packet = demo_data.dequeue();
    }
    if (!demo_data.isEmpty()) {
        AOPacket wait_packet = AOPacket(current_packet);
        int duration = wait_packet.get_contents().at(0).toInt();
        QTimer::singleShot(duration, this, &DemoServer::playback);
    }
}

void DemoServer::client_disconnect()
{
    client_sock->deleteLater();
    client_sock = nullptr;
}
