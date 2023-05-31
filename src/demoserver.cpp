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

void DemoServer::set_demo_file(QString filepath)
{
    filename = filepath;
}

void DemoServer::start_server()
{
    if (server_started) return;
    if (!tcp_server->listen(QHostAddress::LocalHost, 0)) {
        qCritical() << "Could not start demo playback server...";
        qDebug() << tcp_server->errorString();
        return;
    }
    this->port = tcp_server->serverPort();
    qInfo() << "Demo server started at port" << port;
    server_started = true;
}

void DemoServer::destroy_connection()
{
    QTcpSocket* temp_socket = tcp_server->nextPendingConnection();
    connect(temp_socket, &QAbstractSocket::disconnected, temp_socket, &QObject::deleteLater);
    temp_socket->disconnectFromHost();
    return;
}

void DemoServer::accept_connection()
{
    if (filename.isEmpty())
    {
        destroy_connection();
        return;
    }
    load_demo(filename);

    if (demo_data.isEmpty())
    {
        destroy_connection();
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
        qWarning() << "Multiple connections to demo server disallowed.";
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

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    const QStringList packet_list = in_data.split("%", QString::SplitBehavior(QString::SkipEmptyParts));
#else
    const QStringList packet_list = in_data.split("%", Qt::SkipEmptyParts);
#endif

    for (const QString &packet : packet_list) {
        QStringList f_contents;
        // Packet should *always* end with #
        if (packet.endsWith("#")) {
          f_contents = packet.chopped(1).split("#");
        }
        // But, if it somehow doesn't, we should still be able to handle it
        else {
          f_contents = packet.split("#");
        }
        // Empty packets are suspicious!
        if (f_contents.isEmpty()) {
          qWarning() << "WARNING: Empty packet received from server, skipping...";
          continue;
        }
        // Take the first arg as the command
        QString command = f_contents.takeFirst();
        // The rest is contents of the packet
        AOPacket *f_packet = new AOPacket(command, f_contents);
        // Ship it to the server!
        handle_packet(f_packet);
    }
}

void DemoServer::handle_packet(AOPacket *p_packet)
{
    p_packet->net_decode();

    // This code is literally a barebones AO server
    // It is wise to do it this way, because I can
    // avoid touching any of this disgusting shit
    // related to hardcoding this stuff in.

    // Also, at some point, I will make akashit
    // into a shared library.

    QString header = p_packet->get_header();
    QStringList contents = p_packet->get_contents();

    if (header == "HI") {
        client_sock->write("ID#0#DEMOINTERNAL#0#%");
    }
    else if (header == "ID") {
        QStringList feature_list = {
                "noencryption", "yellowtext",       "prezoom",
                "flipping",     "customobjections", "fastloading",
                "deskmod",      "evidence",         "cccc_ic_support",
                "arup",         "casing_alerts",    "modcall_reason",
                "looping_sfx",  "additive",         "effects",
                "y_offset",     "expanded_desk_mods"};
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
        QString packet = "CT#DEMO#" + tr("Demo file loaded. Send /play or > in OOC to begin playback.") + "#1#%";
        client_sock->write(packet.toUtf8());
    }
    else if (header == "CT") {
        if (contents[1].startsWith("/load"))
        {
          QString path = QFileDialog::getOpenFileName(nullptr, tr("Load Demo"), "logs/", tr("Demo Files (*.demo)"));
          if (path.isEmpty())
            return;
          load_demo(path);
          QString packet = "CT#DEMO#" + tr("Demo file loaded. Send /play or > in OOC to begin playback.") + "#1#%";
          client_sock->write(packet.toUtf8());
          reset_state();
        }
        else if (contents[1].startsWith("/play") || contents[1] == ">")
        {
          if (timer->interval() != 0 && !timer->isActive())
          {
            timer->start();
            QString packet = "CT#DEMO#" + tr("Resuming playback.") + "#1#%";
            client_sock->write(packet.toUtf8());
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
          QString packet = "CT#DEMO#" + tr("Pausing playback.") + "#1#%";
          client_sock->write(packet.toUtf8());
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
              QString packet = "CT#DEMO#" + tr("Setting max_wait to") + " ";
              client_sock->write(packet.toUtf8());
              client_sock->write(QString::number(max_wait).toUtf8());
              packet = " " + tr("milliseconds.") + "#1#%";
              client_sock->write(packet.toUtf8());
            }
            else
            {
              QString packet = "CT#DEMO#" + tr("Not a valid integer!") + "#1#%";
              client_sock->write(packet.toUtf8());
            }
          }
          else
          {

              QString packet = "CT#DEMO#" + tr("Current max_wait is") + " ";
              client_sock->write(packet.toUtf8());
              client_sock->write(QString::number(max_wait).toUtf8());
              packet = " " + tr("milliseconds.") + "#1#%";
              client_sock->write(packet.toUtf8());
          }
        }
        else if (contents[1].startsWith("/reload"))
        {
            load_demo(p_path);
            QString packet = "CT#DEMO#" + tr("Current demo file reloaded. Send /play or > in OOC to begin playback.") + "#1#%";
            client_sock->write(packet.toUtf8());
            reset_state();
        }
        else if (contents[1].startsWith("/min_wait"))
        {
            QString packet = "CT#DEMO#" + tr("min_wait is deprecated. Use the client Settings for minimum wait instead!") + "#1#%";
            client_sock->write(packet.toUtf8());
        }
        else if (contents[1].startsWith("/debug"))
        {
            QStringList args = contents[1].split(" ");
            if (args.size() > 1)
            {
              bool ok;
              int toggle = args.at(1).toInt(&ok);
              if (ok && (toggle == 0 || toggle == 1)) {
                debug_mode = toggle == 1;
                QString packet = "CT#DEMO#" + tr("Setting debug mode to %1").arg(static_cast<int>(debug_mode)) + "#1#%";
                client_sock->write(packet.toUtf8());
                // Debug mode disabled?
                if (!debug_mode) {
                  // Reset the timer
                  client_sock->write("TI#4#1#0#%");
                  client_sock->write("TI#4#3#0#%");
                }
              }
              else
              {
                QString packet = "CT#DEMO#" + tr("Valid values are 1 or 0!") + "#1#%";
                client_sock->write(packet.toUtf8());
              }
            }
            else
            {
              QString packet = "CT#DEMO#" + tr("Set debug mode using /debug 1 to enable, and /debug 0 to disable, which will use the fifth timer (TI#4) to show the remaining time until next demo line.") + "#1#%";
              client_sock->write(packet.toUtf8());
            }
        }
        else if (contents[1].startsWith("/help"))
        {
            QString packet = "CT#DEMO#" + tr("Available commands:\nload, reload, play, pause, max_wait, debug, help") + "#1#%";
            client_sock->write(packet.toUtf8());
        }
    }

    delete p_packet;
}

void DemoServer::load_demo(QString filename)
{
    QFile demo_file(filename);
    demo_file.open(QIODevice::ReadOnly);
    if (!demo_file.isOpen())
        return;
    // Clear demo data
    demo_data.clear();
    // Set the demo filepath
    p_path = filename;
    // Process the demo file
    QTextStream demo_stream(&demo_file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    demo_stream.setCodec("UTF-8");
#endif
    QString line = demo_stream.readLine();
    while (!line.isNull()) {
        while (!line.endsWith("%")) {
          line += "\n";
          line += demo_stream.readLine();
        }
        demo_data.enqueue(line);
        line = demo_stream.readLine();
    }
    demo_file.flush();
    demo_file.close();

    // No-shenanigans 2.9.0 demo file with the dreaded demo desync bug detected https://github.com/AttorneyOnline/AO2-Client/pull/496
    // If we don't start with the SC packet this means user-edited weirdo shenanigans. Don't screw around with those.
    if (demo_data.head().startsWith("SC#") && demo_data.last().startsWith("wait#")) {
      qInfo() << "Loaded a broken pre-2.9.1 demo file, with the wait desync issue!";
      QMessageBox *msgBox = new QMessageBox;
      msgBox->setAttribute(Qt::WA_DeleteOnClose);
      msgBox->setTextFormat(Qt::RichText);
      msgBox->setText("This appears to be a <b>broken</b> pre-2.9.1 demo file with the <a href=https://github.com/AttorneyOnline/AO2-Client/pull/496>wait desync issue</a>!<br>Do you want to correct this file? <i>If you refuse, this demo will be desynchronized!</i>");
      msgBox->setWindowTitle("Pre-2.9.1 demo detected!");
      msgBox->setStandardButtons(QMessageBox::NoButton);
      QTimer::singleShot(2000, msgBox, std::bind(&QMessageBox::setStandardButtons,msgBox,QMessageBox::Yes|QMessageBox::No));
      int ret = msgBox->exec();
      QQueue <QString> p_demo_data;
      switch (ret) {
        case QMessageBox::Yes:
          qInfo() << "Making a backup of the broken demo...";
          QFile::copy(filename, filename + ".backup");
          while (!demo_data.isEmpty()) {
            QString current_packet = demo_data.dequeue();
            // TODO: faster way of doing this, maybe with QtConcurrent's MapReduce methods?
            if (!current_packet.startsWith("SC#") && current_packet.startsWith("wait#")) {
              p_demo_data.insert(qMax(1, p_demo_data.size()-1), current_packet);
              continue;
            }
            p_demo_data.enqueue(current_packet);
          }
          if (demo_file.open(QIODevice::WriteOnly | QIODevice::Text |
                         QIODevice::Truncate)) {
            QTextStream out(&demo_file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << p_demo_data.dequeue();
            for (const QString &line : qAsConst(p_demo_data)) {
              out << "\n" << line;
            }
            demo_file.flush();
            demo_file.close();
          }
          load_demo(filename);
          break;
        case QMessageBox::No:
          // No was clicked
          break;
        default:
          // should never be reached
          break;
      }
    }
}

void DemoServer::reset_state()
{
    // Reset evidence list
    client_sock->write("LE##%");

    // Reset timers
    client_sock->write("TI#0#1#0#%");
    client_sock->write("TI#0#3#0#%");
    client_sock->write("TI#1#1#0#%");
    client_sock->write("TI#1#3#0#%");
    client_sock->write("TI#2#1#0#%");
    client_sock->write("TI#2#3#0#%");
    client_sock->write("TI#3#1#0#%");
    client_sock->write("TI#3#3#0#%");
    client_sock->write("TI#4#1#0#%");
    client_sock->write("TI#4#3#0#%");

    // Set the BG to default (also breaks up the message queue)
    client_sock->write("BN#default#wit#%");

    // Stop the wait packet timer
    timer->stop();
}

void DemoServer::playback()
{
    if (demo_data.isEmpty())
      return;

    QString current_packet = demo_data.dequeue();
    // We reset the elapsed time with this packet
    if (current_packet.startsWith("MS#"))
      elapsed_time = 0;

    while (!current_packet.startsWith("wait#")) {
        client_sock->write(current_packet.toUtf8());
        if (demo_data.isEmpty())
          break;
        current_packet = demo_data.dequeue();
    }
    if (!demo_data.isEmpty()) {
        QStringList f_contents;
        // Packet should *always* end with #
        if (current_packet.endsWith("#")) {
          f_contents = current_packet.chopped(1).split("#");
        }
        // But, if it somehow doesn't, we should still be able to handle it
        else {
          f_contents = current_packet.split("#");
        }
        // Take the first arg as the command
        QString command = f_contents.takeFirst();
        int duration = 0;
        if (!f_contents.isEmpty()) {
          duration = f_contents.at(0).toInt();
        }
        // Max wait reached
        if (max_wait != -1 && duration + elapsed_time > max_wait) {
          int prev_duration = duration;
          duration = qMax(0, max_wait - elapsed_time);
          qDebug() << "Max_wait of " << max_wait << " reached. Forcing duration to " << duration << "ms";
          // Skip the difference on the timers
          emit skip_timers(prev_duration - duration);
        }
        // Manual user skip, such as with >
        else if (timer->remainingTime() > 0) {
          qDebug() << "Timer of interval " << timer->interval() << " is being skipped. Forcing to skip " << timer->remainingTime() << "ms on TI# clocks";
          emit skip_timers(timer->remainingTime());
        }
        elapsed_time += duration;
        timer->start(duration);
        if (debug_mode) {
          client_sock->write("TI#4#2#%");
          QString debug_timer = "TI#4#0#" + QString::number(duration) + "#%";
          client_sock->write(debug_timer.toUtf8());
        }
    }
    else {
      QString end_packet = "CT#DEMO#" + tr("Reached the end of the demo file. Send /play or > in OOC to restart, or /load to open a new file.") + "#1#%";
      client_sock->write(end_packet.toUtf8());
      timer->setInterval(0);
    }
}

void DemoServer::client_disconnect()
{
    client_sock->deleteLater();
    client_sock = nullptr;
}
