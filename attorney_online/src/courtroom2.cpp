#include "courtroom2.h"

#include <QDockWidget>


#include "bass.h"
#include "qmenu.h"

Courtroom2::Courtroom2(std::shared_ptr<Client> client, AOApplication *p_ao_app) :
    QMainWindow(), ao_app(p_ao_app), client(client)
{
    setWindowTitle("Attorney Online 2");
    setWindowIcon(QIcon(":/logo.png"));
    setAttribute(Qt::WA_DeleteOnClose, true);
}

Courtroom2::~Courtroom2()
{

}

void Courtroom2::setupCourtroom()
{

}

void Courtroom2::initBass()
{
      BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
      BASS_Free();
      // Change the default audio output device to be the one the user has given
      // in his config.ini file for now.
      unsigned int a = 0;
      BASS_DEVICEINFO info;

      if (options.audioOutputDevice() == "default") {
        BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
        BASS_PluginLoad("bassopus.dll", 0);
        BASS_PluginLoad("bassmidi.dll", 0);
      }
      else {
        for (a = 0; BASS_GetDeviceInfo(a, &info); a++) {
          if (options.audioOutputDevice() == info.name) {
            BASS_SetDevice(a);
            BASS_Init(static_cast<int>(a), 48000, BASS_DEVICE_LATENCY, nullptr,
                      nullptr);
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

template<typename T>
void Courtroom2::registerWindow(T *&widget, const QString &name, QDockWidget *&dockWidget, QAction *&toggleAction)
{
    widget = findChild<T *>(name);
      dockWidget = findChild<QDockWidget *>(name + "_dock");
      toggleAction = findChild<QAction *>("toggle_" + name);
      if (widget == nullptr)
      {
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
      else
      {
        toggleAction->setChecked(true);
      }
      connect(toggleAction, &QAction::toggled, this, [=](bool toggled) {
        dockWidget->setVisible(toggled);
      });
      connect(ui_window_menu, &QMenu::aboutToShow, this, [=] {
        toggleAction->setChecked(dockWidget->isVisible());
      });

      windows.insert(dockWidget);
}
