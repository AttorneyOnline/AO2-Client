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
  DirectConnectDialog(NetworkManager *p_net_manager);
  ~DirectConnectDialog() = default;

private Q_SLOTS:
  void onConnectPressed();
  void onServerConnected();
  void onConnectTimeout();

private:
  NetworkManager *net_manager;

  QLineEdit *ui_direct_hostname_edit;

  QLabel *ui_direct_connection_status_lbl;
  QPushButton *ui_direct_connect_button;
  QPushButton *ui_direct_cancel_button;

  QWidget *ui_widget;
  QTimer connect_timeout;

  const int TCP_INDEX = 0;
  const QRegularExpression SCHEME_PATTERN{"^\\w+://.+$"};
  const int CONNECT_TIMEOUT = 5 * 1000;
  const QString DEFAULT_UI = "direct_connect_dialog.ui";
};
