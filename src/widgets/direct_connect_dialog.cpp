#include "direct_connect_dialog.h"

#include "debug_functions.h"
#include "gui_utils.h"
#include "networkmanager.h"
#include "options.h"

#include <QStringBuilder>
#include <QUiLoader>
#include <QVBoxLayout>

const QString DirectConnectDialog::UI_FILE_PATH = "direct_connect_dialog.ui";
const QRegularExpression DirectConnectDialog::SCHEME_PATTERN{"^\\w+://.+$"};
const int DirectConnectDialog::CONNECT_TIMEOUT = 5 * 1000;

DirectConnectDialog::DirectConnectDialog(NetworkManager *netManager, QWidget *parent)
    : QDialog(parent)
    , net_manager(netManager)
{
  QUiLoader l_loader(this);
  QFile l_uiFile(Options::getInstance().getUIAsset(UI_FILE_PATH));

  if (!l_uiFile.open(QFile::ReadOnly))
  {
    qCritical() << "Unable to open file " << l_uiFile.fileName();
    return;
  }
  ui_widget = l_loader.load(&l_uiFile, this);

  auto l_layout = new QVBoxLayout(this);
  l_layout->addWidget(ui_widget);

  FROM_UI(QLineEdit, direct_hostname_edit);

  FROM_UI(QLabel, direct_connection_status_lbl);

  FROM_UI(QPushButton, direct_connect_button);
  connect(ui_direct_connect_button, &QPushButton::pressed, this, &DirectConnectDialog::onConnectPressed);
  FROM_UI(QPushButton, direct_cancel_button);
  connect(ui_direct_cancel_button, &QPushButton::pressed, this, &DirectConnectDialog::close);

  connect(net_manager, &NetworkManager::server_connected, this, &DirectConnectDialog::onServerConnected);

  connect(&m_connect_timeout, &QTimer::timeout, this, &DirectConnectDialog::onConnectTimeout);
  m_connect_timeout.setSingleShot(true);
}

void DirectConnectDialog::onConnectPressed()
{
  QString l_hostname = ui_direct_hostname_edit->text();
  if (!SCHEME_PATTERN.match(l_hostname).hasMatch())
  {
    l_hostname = "ws://" % l_hostname;
  }

  QUrl l_url(l_hostname);
  if (!l_url.isValid())
  {
    call_error(tr("Invalid URL."));
    return;
  }

  if (l_url.scheme() != "ws")
  {
    call_error(tr("Invalid URL scheme. Only ws:// is supported."));
    return;
  }

  if (l_url.port() == -1)
  {
    call_error(tr("Invalid server port."));
    return;
  }
  ServerInfo l_server;
  l_server.address = l_url.host();
  l_server.port = l_url.port();
  l_server.name = "Direct Connection";

  net_manager->connect_to_server(l_server);
  ui_direct_connect_button->setEnabled(false);
  ui_direct_connection_status_lbl->setText("Connecting...");
  ui_direct_connection_status_lbl->setStyleSheet("color : rgb(0,64,156)");
  m_connect_timeout.start(CONNECT_TIMEOUT);
}

void DirectConnectDialog::onServerConnected()
{
  net_manager->join_to_server();
  ui_direct_connection_status_lbl->setText("Connected!");
  ui_direct_connection_status_lbl->setStyleSheet("color: rgb(0,128,0)");
  close();
}

void DirectConnectDialog::onConnectTimeout()
{
  ui_direct_connect_button->setEnabled(true);
  ui_direct_connection_status_lbl->setText("Connection Timeout!");
  ui_direct_connection_status_lbl->setStyleSheet("color: rgb(255,0,0)");
}
