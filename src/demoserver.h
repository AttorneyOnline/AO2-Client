#pragma once

#include "aopacket.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QWebSocket>
#include <QWebSocketServer>

class DemoServer : public QObject
{
  Q_OBJECT

public:
  explicit DemoServer(QObject *parent = nullptr);

  int port();

  void set_demo_file(QString filepath);

private:
  bool m_server_started = false;
  int m_port = 0;
  int m_max_wait = -1;

  QWebSocketServer *server;
  QWebSocket *client_sock = nullptr;
  bool client_connected = false;
  bool partial_packet = false;
  bool debug_mode = false;
  QString temp_packet;
  QQueue<QString> demo_data;
  QString sc_packet;
  int num_chars = 0;
  QString p_path;
  QTimer *timer;
  int elapsed_time = 0;
  QString filename;

  void handle_packet(AOPacket packet);
  void load_demo(QString filename);
  void reset_state();

private Q_SLOTS:
  void accept_connection();
  void destroy_connection();
  void recv_data(const QString &in_data);
  void client_disconnect();
  void playback();

public Q_SLOTS:
  void start_server();

Q_SIGNALS:
  void skip_timers(qint64 msecs);
};
