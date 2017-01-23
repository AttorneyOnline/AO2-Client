#include "aoapplication.h"

#include "lobby.h"
#include "networkmanager.h"
#include "encryption_functions.h"
#include "hardware_functions.h"

#include <QDebug>

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
    QString message_line;

    if (f_contents.size() == 1)
      message_line = f_contents.at(0);
    else if (f_contents.size() >= 2)
      message_line = f_contents.at(0) + ": " + f_contents.at(1);
    else
      return;

    if (lobby_constructed)
    {
      w_lobby->append_chatmessage(message_line);
    }
    if (courtroom_constructed)
    {
      //T0D0: uncomment this when it's implemented
      //w_courtroom->append_ms_chat_message(message_line);
    }
  }
}

void AOApplication::server_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();
  QString f_packet = p_packet->to_string();

  qDebug() << "R: " << f_packet;

  if (header == "decryptor")
  {
    if (f_contents.size() == 0)
      return;

    //you may ask where 322 comes from. that would be a good question.
    s_decryptor = fanta_decrypt(f_contents.at(0), 322).toUInt();

    QString f_hdid;
    f_hdid = get_hdid();

    AOPacket *hi_packet = new AOPacket("HI#" + f_hdid + "#%");
    send_server_packet(hi_packet);

    delete hi_packet;
  }
  else if (header == "ID")
  {
    if (f_contents.size() < 1)
      return;

    //T0D0: save server version here, somehow
  }
  else if (header == "CT")
  {
    if (f_contents.size() < 2)
    {
      qDebug() << "W: malformed packet!";
      return;
    }

    //QString message_line = f_contents.at(0) + ": " + f_contents.at(1);

    //T0D0, uncomment when implemented
    //w_courtroom->append_ooc_chatmessage(message_line)
  }
  else if (header == "PN")
  {
    if (f_contents.size() < 2)
      return;

    w_lobby->set_player_count(f_contents.at(0).toInt(), f_contents.at(1).toInt());
  }
}

void AOApplication::send_ms_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  net_manager->ship_ms_packet(f_packet);

  qDebug() << "S(ms):" << f_packet;
}

void AOApplication::send_server_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  if (encryption_needed)
  {
    qDebug() << "S:(e)" << f_packet;

    p_packet->encrypt_header(s_decryptor);
    f_packet = p_packet->to_string();
  }
  else
  {
    qDebug() << "S:" << f_packet;
  }

  net_manager->ship_server_packet(f_packet);

}
