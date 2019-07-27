#include "widgets/courtroom.h"
#include "lobby.h"

#include "file_functions.h"
#include "debug_functions.h"
#include "hardware_functions.h"

void Courtroom::char_clicked(int n_char)
{
  QString char_ini_path = ao_app->get_character_path(char_list.at(n_char).name, "char.ini");

  qDebug() << "char_ini_path" << char_ini_path;

  if (!file_exists(char_ini_path))
  {
    call_notice("Could not find " + char_ini_path);
    return;
  }

  if (n_char == m_cid)
  {
    enter_courtroom(m_cid);
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("CC#" + QString::number(ao_app->s_pv) + "#" + QString::number(n_char) + "#" + get_hdid() + "#%"));
  }

  ui_ic_chat_name->setPlaceholderText(char_list.at(n_char).name);
}

