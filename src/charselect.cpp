#include "courtroom.h"
#include "lobby.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "widgets/aocharselect.hpp"

void Courtroom::char_clicked(int n_char)
{
  if (n_char != m_cid || n_char == -1) {
    ao_app->send_server_packet(
        new AOPacket("CC", {QString::number(ao_app->client_id),
                            QString::number(n_char), get_hdid()}));
  }
  if (n_char == m_cid || n_char == -1) {
    update_character(n_char);
    enter_courtroom();
  }
}

void Courtroom::open_charselect()
{
  if (ui_charselect == nullptr) {
    ui_charselect = new AOCharSelect(centralWidget(), ao_app);
    ui_charselect->loadUI(char_list);
    connect(ui_charselect, &AOCharSelect::characterSelected, this,
            &Courtroom::char_clicked);
    connect(this, &Courtroom::closeCharselect, ui_charselect,
            &AOCharSelect::close);
    connect(ui_charselect, &AOCharSelect::destroyed, this,
            [=] { ui_charselect = nullptr; });
    ui_charselect->show();
  }
}
