#ifndef COURTROOM2_H
#define COURTROOM2_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QVector>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QMap>
#include <QTextBrowser>
#include <QSpinBox>

#include <QDebug>
#include <QScrollBar>
#include <QRegExp>
#include <QBrush>
#include <QTextCharFormat>
#include <QFont>
#include <QInputDialog>
#include <QFileDialog>
#include <QAction>

#include "network/client.h"
#include "options.h"

using namespace AttorneyOnline;

class AOApplication;
class AOViewport;

class Courtroom2 : public QMainWindow {
    Q_OBJECT
public:
    Courtroom2(std::shared_ptr<Client> client, AOApplication* p_ao_app = nullptr);
    ~Courtroom2();

    void setupCourtroom(QString p_server_name = "NoName");

private:
    AOApplication* ao_app;
    std::shared_ptr<Client> client;
    Options options;

    QString previous_ic_message = "";

    // Whether or not the character selection screen will quit the server
    // when canceling.
    bool quitOnCancel = true;

    // List of registered windows; used for saving/loading layout
    QSet<QWidget *> windows;
    const QString LAYOUT_AUTOSAVE_FILE = "autosave_layout.dat";
    const int LAYOUT_AUTOSAVE_INTERVAL = 30000;

    QMainWindow *windowWidget;
    AOViewport* viewport;

    // Registers dockedWidget to the QMenu Action for the widget to hide/show it.
    template<typename T>
    void registerWindow(T *&widget, const QString &name,
                        QDockWidget *&dockWidget, QAction *&toggleAction);

    void initBass();

    QMenu *ui_window_menu;
    QAction *ui_load_layout;
    QAction *ui_save_layout;

private slots:
};

#endif
