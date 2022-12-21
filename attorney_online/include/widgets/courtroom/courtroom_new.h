#ifndef COURTROOM_NEW_H
#define COURTROOM_NEW_H

#include "include/network/legacyclient.h"
#include "options.h"

#include <QMainWindow>

#define REGISTER_WINDOW(type, name)                                            \
  registerWindow<type>(ui_##name, #name, ui_##name##_dock, ui_toggle_##name);

using namespace AttorneyOnline;

class AOApplication;
class AOViewport;

class Courtroom_NEW : public QMainWindow {
  Q_OBJECT

public:
  /**
   * @brief Constructor for the primary gameplay class, Courtroom_NEW
   * @param Pointer to AOApplication for pathing
   * @param Network Client for client <-> server communication.
   */
  explicit Courtroom_NEW(AOApplication *p_ao_app,
                         std::shared_ptr<Client> net_client);

  /**
   * @brief Sets up the entire UI and registers windows to the main courtroom.
   *
   */
  void setupCourtroom();

public slots:
  /**
   * @brief Reloads the widgets of the Courtroom.
   *
   * @details Due to the nature of AOs subthemes and hot theme reloading we need
   * to be able to thrash our UI and keep the data, yet also load our new
   * assets.
   *
   * @note You may wonder why this is a slot.
   */
  void reloadCourtroom();

private:
  AOApplication *ao_app;
  Options options;

  std::shared_ptr<Client> client;
};

#endif // COURTROOM_NEW_H
