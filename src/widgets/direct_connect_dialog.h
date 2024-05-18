#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTimer>

class NetworkManager;

class DirectConnectDialog : public QDialog
{
  Q_OBJECT

public:
  DirectConnectDialog(NetworkManager *netManager, QWidget *parent = nullptr);

private:
  static const QString UI_FILE_PATH;
  static const QRegularExpression SCHEME_PATTERN;
  static const int CONNECT_TIMEOUT;

  NetworkManager *net_manager;
  QTimer m_connect_timeout;

  QWidget *ui_widget;

  QLineEdit *ui_direct_hostname_edit;

  QLabel *ui_direct_connection_status_lbl;
  QPushButton *ui_direct_connect_button;
  QPushButton *ui_direct_cancel_button;

private Q_SLOTS:
  void onConnectPressed();
  void onServerConnected();
  void onConnectTimeout();
};
