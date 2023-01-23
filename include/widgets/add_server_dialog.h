#ifndef ADD_SERVER_DIALOG_H
#define ADD_SERVER_DIALOG_H

#include <QDialog>
#include <QString>
class QPushButton;
class QDialogButton;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QPlainTextEdit;
class QDialogButtonBox;
class QSettings;

class AddServerDialog : public QDialog {
    Q_OBJECT
public :
    AddServerDialog();
    ~AddServerDialog();

private:
    QWidget* ui_widget;

    QLineEdit* ui_server_display_name_edit;
    QLineEdit* ui_server_hostname_edit;
    QSpinBox* ui_server_port_box;
    QComboBox* ui_server_protocol_box;
    QPlainTextEdit* ui_server_description_edit;
    QDialogButtonBox* ui_server_dialog_button;

    // Legacy Server UI
    QLineEdit* ui_server_legacy_edit;
    QPushButton* ui_server_legacy_load_button;


    QSettings* m_favorite_ini;

    const int TCP_INDEX = 0;

    const QString DEFAULT_UI = ":/resource/ui/add_server_dialog.ui";

private slots:
    void savePressed();
    void discardPressed();
    void parseLegacyServerEntry();
};

#endif // ADD_SERVER_DIALOG_H
