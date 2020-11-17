#include "demoserver.h"
#include "lobby.h"

DemoServer::DemoServer(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setSingleShot(true);

    tcp_server = new QTcpServer(this);
    connect(tcp_server, &QTcpServer::newConnection, this, &DemoServer::accept_connection);
    connect(timer, &QTimer::timeout, this, &DemoServer::playback);
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
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Load Demo"), "logs/", tr("Demo Files (*.demo)"));
    if (path.isEmpty())
    {
      QTcpSocket* temp_socket = tcp_server->nextPendingConnection();
      connect(temp_socket, &QAbstractSocket::disconnected, temp_socket, &QObject::deleteLater);
      temp_socket->disconnectFromHost();
      return;
    }
    load_demo(path);
    if (demo_data.isEmpty())
    {
      QTcpSocket* temp_socket = tcp_server->nextPendingConnection();
      connect(temp_socket, &QAbstractSocket::disconnected, temp_socket, &QObject::deleteLater);
      temp_socket->disconnectFromHost();
      return;
    }

    if (demo_data.head().startsWith("SC#"))
    {
      sc_packet = demo_data.dequeue();
      AOPacket sc(sc_packet);
      num_chars = sc.get_contents().length();
    }
    else
    {
      sc_packet = "SC#%";
      num_chars = 0;
    }

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
        client_sock->write("SM#%");
    }
    else if (header == "RD") {
        client_sock->write("DONE#%");
    }
    else if (header == "CC") {
        client_sock->write("PV#0#CID#-1#%");
    }
    else if (header == "CT") {
        if (contents[1].startsWith("/load"))
        {
          QString path = QFileDialog::getOpenFileName(nullptr, tr("Load Demo"), "logs/", tr("Demo Files (*.demo)"));
          if (path.isEmpty())
            return;
          load_demo(path);
          client_sock->write("CT#DEMO#Demo file loaded. Send /play or > in OOC to begin playback.#1#%");
        }
        else if (contents[1].startsWith("/play") || contents[1] == ">")
        {
          if (timer->interval() != 0 && !timer->isActive())
          {
            timer->start();
            client_sock->write("CT#DEMO#Resuming playback.#1#%");
          }
          else
          {
            if (demo_data.isEmpty() && p_path != "")
              load_demo(p_path);
            playback();
          }
        }
        else if (contents[1].startsWith("/pause") || contents[1] == "|")
        {
          int timeleft = timer->remainingTime();
          timer->stop();
          timer->setInterval(timeleft);
          client_sock->write("CT#DEMO#Pausing playback.#1#%");
        }
        else if (contents[1].startsWith("/max_wait"))
        {
          QStringList args = contents[1].split(" ");
          if (args.size() > 1)
          {
            bool ok;
            int p_max_wait = args.at(1).toInt(&ok);
            if (ok)
            {
              if (p_max_wait < 0)
                p_max_wait = -1;
              max_wait = p_max_wait;
              client_sock->write("CT#DEMO#Setting max_wait to ");
              client_sock->write(QString::number(max_wait).toUtf8());
              client_sock->write(" milliseconds.#1#%");
            }
            else
            {
              client_sock->write("CT#DEMO#Not a valid integer!#1#%");
            }
          }
          else
          {
            client_sock->write("CT#DEMO#Current max_wait is ");
            client_sock->write(QString::number(max_wait).toUtf8());
            client_sock->write(" milliseconds.#1#%");
          }
        }
        else if (contents[1].startsWith("/min_wait"))
        {
          QStringList args = contents[1].split(" ");
          if (args.size() > 1)
          {
            bool ok;
            int p_min_wait = args.at(1).toInt(&ok);
            if (ok)
            {
              if (p_min_wait < 0)
                p_min_wait = -1;
              min_wait = p_min_wait;
              client_sock->write("CT#DEMO#Setting min_wait to ");
              client_sock->write(QString::number(min_wait).toUtf8());
              client_sock->write(" milliseconds.#1#%");
            }
            else
            {
              client_sock->write("CT#DEMO#Not a valid integer!#1#%");
            }
          }
          else
          {
            client_sock->write("CT#DEMO#Current min_wait is ");
            client_sock->write(QString::number(min_wait).toUtf8());
            client_sock->write(" milliseconds.#1#%");
          }
        }
    }
}

void DemoServer::load_demo(QString filename)
{
    QFile demo_file(filename);
    demo_file.open(QIODevice::ReadOnly);
    if (!demo_file.isOpen())
        return;
    demo_data.clear();
    p_path = filename;
    QTextStream demo_stream(&demo_file);
    QString line = demo_stream.readLine();
    while (!line.isNull()) {
        if (!line.endsWith("%")) {
          line += "\n";
        }
        demo_data.enqueue(line);
        line = demo_stream.readLine();
    }
}

void DemoServer::playback()
{
    if (demo_data.isEmpty())
      return;

    QString current_packet = demo_data.dequeue();
    // We reset the elapsed time with this packet
    if (current_packet.startsWith("MS#"))
      elapsed_time = 0;

    while (!current_packet.startsWith("wait") && !demo_data.isEmpty()) {
        client_sock->write(current_packet.toUtf8());
        current_packet = demo_data.dequeue();
    }
    if (!demo_data.isEmpty()) {
        AOPacket wait_packet = AOPacket(current_packet);

        int duration = wait_packet.get_contents().at(0).toInt();
        if (max_wait != -1 && duration + elapsed_time > max_wait)
          duration = qMax(0, max_wait - elapsed_time);
        // We use elapsed_time to make sure that the packet we're using min_wait on is "priority" (e.g. IC)
        if (elapsed_time == 0 && min_wait != -1 && duration < min_wait)
          duration = min_wait;
        elapsed_time += duration;
        timer->start(duration);
    }
    else
    {
      client_sock->write("CT#DEMO#Reached the end of the demo file. Send /play or > in OOC to restart, or /load to open a new file.#1#%");
      timer->setInterval(0);
    }
}

void DemoServer::client_disconnect()
{
    client_sock->deleteLater();
    client_sock = nullptr;
}
