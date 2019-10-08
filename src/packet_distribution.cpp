#include "aoapplication.h"

#include "lobby.h"
#include "widgets/courtroom.h"
#include "networkmanager.h"
#include "encryption_functions.h"
#include "hardware_functions.h"
#include "debug_functions.h"

void AOApplication::ms_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();

  if (header != "CHECK")
    qDebug() << "R(ms):" << p_packet->to_string();

  if (header == "ALL")
  {
    server_list.clear();

    for (QString i_string : p_packet->get_contents())
    {
      server_type f_server;
      QStringList sub_contents = i_string.split("&");

      if (sub_contents.size() < 4)
      {
        qDebug() << "W: malformed packet";
        continue;
      }

      f_server.name = sub_contents.at(0);
      f_server.desc = sub_contents.at(1);
      f_server.ip = sub_contents.at(2);
      f_server.port = sub_contents.at(3).toInt();

      server_list.append(f_server);
    }

    if (lobby_constructed)
    {
      w_lobby->list_servers();
    }
  }
  else if (header == "CT")
  {
    QString f_name, f_message;

    if (f_contents.size() == 1)
    {
      f_name = "";
      f_message = f_contents.at(0);
    }
    else if (f_contents.size() >= 2)
    {
      f_name = f_contents.at(0);
      f_message = f_contents.at(1);
    }
    else
      goto end;

    if (lobby_constructed)
    {
      w_lobby->append_chatmessage(f_name, f_message);
    }
    if (courtroom_constructed && courtroom_loaded)
    {
      w_courtroom->append_ms_chatmessage(f_name, f_message);
    }
  }
  else if (header == "AO2CHECK")
  {
    send_ms_packet(new AOPacket("ID#AO2#" + get_version_string() + "#%"));
    send_ms_packet(new AOPacket("HI#" + hdid() + "#%"));

    if (f_contents.size() < 1)
      goto end;

    QStringList version_contents = f_contents.at(0).split(".");

    if (version_contents.size() < 3)
      goto end;

    int f_release = version_contents.at(0).toInt();
    int f_major = version_contents.at(1).toInt();
    int f_minor = version_contents.at(2).toInt();

    if (get_release() > f_release)
      goto end;
    else if (get_release() == f_release)
    {
      if (get_major_version() > f_major)
        goto end;
      else if (get_major_version() == f_major)
      {
        if (get_minor_version() >= f_minor)
          goto end;
      }
    }

    call_notice("Outdated version! Your version: " + get_version_string()
                + "\nPlease go to aceattorneyonline.com to update.");
    destruct_courtroom();
    destruct_lobby();
  }
  else if (header == "DOOM")
  {
    call_notice("You have been exiled from AO.\n"
                "Have a nice day.");
    destruct_courtroom();
    destruct_lobby();
  }

  end:

  delete p_packet;
}
