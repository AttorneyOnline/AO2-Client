#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "aoapplication.h"
#include "aopacket.h"

#include <QDateTime>
#include <QDnsLookup>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QtWebSockets/QWebSocket>
#include <QTime>
#include <QTimer>

#include <cstring>

enum MSDocumentType {
  PrivacyPolicy,
  Motd,
  ClientVersion
};

class NetworkManager : public QObject {
  Q_OBJECT

private:
  AOApplication *ao_app;
  QNetworkAccessManager *http;

  QNetworkAccessManager *stream;
  QNetworkAccessManager *download;


  union {
    QWebSocket *ws;
    QTcpSocket *tcp;
  } server_socket = {nullptr};
  connection_type active_connection_type = TCP;
  bool connected = false;

  QTimer *heartbeat_timer;

  const QString DEFAULT_MS_BASEURL = "http://servers.aceattorneyonline.com";
  QString ms_baseurl = DEFAULT_MS_BASEURL;

  const int heartbeat_interval = 60 * 5 * 1000;

  // Connection liveness. The probe is owned by NetworkManager so it survives
  // courtroom construction/destruction and always matches the live socket.
  QTimer *liveness_timer = nullptr;
  QTimer *liveness_deadline = nullptr;
  QElapsedTimer liveness_ping_timer;
  bool probe_outstanding = false;
  // TCP servers only get a hard CHECK deadline once one has actually replied
  // to CH. The bundled demo server never implements CH, so it is never armed.
  bool server_supports_check = false;
  bool intentional_disconnect = false;
  bool disconnect_notice_shown = false;
  bool send_failure_reported = false;
  qint64 last_inbound_wall_ms = 0;
  qint64 last_latency_ms = -1;
  int character_id = -1;

  static constexpr int kHeartbeatMs = 20'000;
  static constexpr int kProbeTimeoutMs = 15'000;
  static constexpr int kMaxTcpBufferBytes = 1024 * 1024;

  // Raw TCP bytes, kept undecoded until a complete '%'-terminated packet is
  // available so multi-byte UTF-8 characters survive chunk boundaries.
  QByteArray tcp_buffer;
  bool partial_packet = false;
  QString temp_packet = "";

  unsigned int s_decryptor = 5;

  void start_liveness_monitor();
  void stop_liveness_monitor();
  void reset_connection_state();
  bool is_connection_active() const;
  void probe_connection(bool force);

public:
  explicit NetworkManager(AOApplication *parent);
  ~NetworkManager() = default;

  void connect_to_server(server_type p_server);
  void disconnect_from_server();

  server_type last_server_chosen;
  bool established_connection = false;

  void set_character_id(int p_id) { character_id = p_id; }
  qint64 last_ping_latency() const { return last_latency_ms; }

  ////// Streaming variables //////
  QImage streamed_image;
  QPixmap streamed_pixmap;
  QString streamed_path;
  QString streamed_charname;
  bool streaming_successful = false;
  ////////////////////////////////

signals:
  void server_connected(bool state);
  void server_ready();
  void imageLoaded(const QImage& image);
  // A connection that was considered established has been lost. Emitted at
  // most once per connection.
  void connectionLost(const QString &reason);
  // A packet was dropped because the socket is not connected. Emitted at most
  // once per connection to avoid spamming the user.
  void packetSendFailed();

public slots:
  void get_server_list(const std::function<void()> &cb);
  void ship_server_packet(QString p_packet);
  void join_to_server();
  void handle_server_packet(const QString& p_data);

  void request_document(MSDocumentType document_type,
                        const std::function<void(QString)> &cb);
  void start_image_streaming(QString path, QString prefix);
  void download_folder(const QStringList& paths);

  void send_heartbeat();
private slots:
  void ms_request_finished(QNetworkReply *reply,
                           const std::function<void()> &cb);
  void image_reply_finished(QNetworkReply *reply);
  void save_folder(const QByteArray& folderData, const QString& pathUrl, const QString& localFolderPath_r);
  void send_liveness_probe();
  void on_liveness_deadline();
  void on_check_received();
  void on_transport_disconnected();
  void handle_connection_lost(const QString &reason);

private:
  QString get_user_agent() const {
    return QStringLiteral("AttorneyOnline/%1 (Desktop)")
        .arg(ao_app->get_version_string());
  }
};

#endif // NETWORKMANAGER_H
