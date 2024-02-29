#ifndef DIRECT_CONNECT_DIALOG_H
#define DIRECT_CONNECT_DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QRegularExpression>

class QLabel;
class QSpinBox;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class NetworkManager;

class DirectConnectDialog : public QDialog {
    Q_OBJECT
public:
    DirectConnectDialog(NetworkManager* p_net_manager);
    ~DirectConnectDialog() = default;

private slots:
    void onConnectPressed();
    void onServerConnected();
    void onConnectTimeout();

private:
    NetworkManager* net_manager;

    QLineEdit* ui_direct_hostname_edit;

    QLabel* ui_direct_connection_status_lbl;
    QPushButton* ui_direct_connect_button;
    QPushButton* ui_direct_cancel_button;

    QWidget* ui_widget;
    QTimer connect_timeout;

    const int TCP_INDEX = 0;
    const QRegularExpression SCHEME_PATTERN{"^\\w+://.+$"};
    const int CONNECT_TIMEOUT = 5 * 1000;
    const QString DEFAULT_UI = "direct_connect_dialog.ui";;

};

#endif // DIRECT_CONNECT_DIALOG_H
