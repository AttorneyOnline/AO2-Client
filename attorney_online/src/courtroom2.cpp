#include "courtroom2.h"

#include <QDockWidget>
#include <QUiLoader>
#include <QFile>
#include <QMessageBox>
#include <QLayout>

#include "bass.h"
#include "qmenu.h"
#include "widgets/courtroom/aoviewport.h"
#include "aouiloder.h"

Courtroom2::Courtroom2(std::shared_ptr<Client> client, AOApplication *p_ao_app)
    : ao_app(p_ao_app), client(client)
{
  setWindowIcon(QIcon(":/ressource/logo_ao2.png"));
  setAttribute(Qt::WA_DeleteOnClose, true);
}

Courtroom2::~Courtroom2() {}

void Courtroom2::setupCourtroom(QString p_server_name)
{
    setWindowTitle(p_server_name);
    AOUiLoader l_loader(this, ao_app);
    QFile uiFile(":/resource/ui/courtroom.ui");
    if (!uiFile.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, "Unable to open Courtroom", "Unable to open Courtroom UI file. Aborting operation.");
        // int kill = 0 / 0;
        return;
    }

    windowWidget = static_cast<QMainWindow*>(l_loader.load(&uiFile, this));
    windowWidget->setWindowFlag(Qt::Window, false);
    windowWidget->setWindowFlag(Qt::Widget);
    windowWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    windowWidget->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    windowWidget->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    viewport = findChild<AOViewport*>("viewport");
    qDebug() << viewport->sizePolicy();
}

void Courtroom2::initBass()
{
  BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
  BASS_Free();
  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  unsigned int audio_device = 0;
  BASS_DEVICEINFO info;

  if (options.audioOutputDevice() == "default") {
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    BASS_PluginLoad("bassopus.dll", 0);
    BASS_PluginLoad("bassmidi.dll", 0);
  }
  else {
    for (audio_device = 0;
         BASS_GetDeviceInfo(audio_device, &info);
         audio_device++) {
      if (options.audioOutputDevice() == info.name) {
        BASS_SetDevice(audio_device);
        BASS_Init(static_cast<int>(audio_device), 48000,
                  BASS_DEVICE_LATENCY, nullptr, nullptr);
        BASS_PluginLoad("bassopus.dll", 0);
        BASS_PluginLoad("bassmidi.dll", 0);
        qInfo() << info.name << "was set as the default audio output device.";
        return;
      }
    }
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    BASS_PluginLoad("bassopus.dll", 0);
    BASS_PluginLoad("bassmidi.dll", 0);
  }
}

template <typename T>
void Courtroom2::registerWindow(T *&widget, const QString &name,
                                QDockWidget *&dockWidget,
                                QAction *&toggleAction)
{
  widget = findChild<T *>(name);
  dockWidget = findChild<QDockWidget *>(name + "_dock");
  toggleAction = findChild<QAction *>("toggle_" + name);
  if (widget == nullptr) {
    dockWidget = new QDockWidget(this);
    widget = new T(dockWidget, ao_app);
    widget->setObjectName(name);
    dockWidget->setObjectName(name + "_dock");
    dockWidget->setWidget(widget);
    dockWidget->setWindowTitle(widget->windowTitle());
    windowWidget->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
    dockWidget->setFloating(true);
    dockWidget->setVisible(false);
  }
  else {
    toggleAction->setChecked(true);
  }
  connect(toggleAction, &QAction::toggled, this,
          [=](bool toggled) { dockWidget->setVisible(toggled); });
  connect(ui_window_menu, &QMenu::aboutToShow, this,
          [=] { toggleAction->setChecked(dockWidget->isVisible()); });

  windows.insert(dockWidget);
}
