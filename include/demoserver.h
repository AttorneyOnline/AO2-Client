#ifndef DEMOSERVER_H
#define DEMOSERVER_H

#include "aopacket.h"

#include <QDebug>
#include <QObject>
#include <QQueue>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QFileDialog>

class DemoServer : public QObject
{
    Q_OBJECT
public:
    explicit DemoServer(QObject *parent = nullptr);

    bool server_started = false;
    int port = 27088;
    int max_wait = -1;
    int min_wait = -1;

private:
    void handle_packet(AOPacket packet);
    void load_demo(QString filename);

    QTcpServer* tcp_server;
    QTcpSocket* client_sock = nullptr;
    bool client_connected = false;
    bool partial_packet = false;
    QString temp_packet = "";
    QQueue<QString> demo_data;
    QString sc_packet;
    int num_chars = 0;
    QString p_path;
    QTimer *timer;
    int elapsed_time = 0;

private slots:
    void accept_connection();
    void recv_data();
    void client_disconnect();
    void playback();

public slots:
    void start_server();

signals:

};

#endif // DEMOSERVER_H
