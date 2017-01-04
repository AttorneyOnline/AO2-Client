#include "aoapplication.h"

#include "lobby.h"

#include <QDebug>

void AOApplication::ms_packet_received(AOPacket *p_packet)
{
  QString header = p_packet->get_header();

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
        qDebug() << "W: malformed packet!";
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
    QStringList f_contents = p_packet->get_contents();

    QString message_line;

    if (f_contents.size() == 1)
      message_line = f_contents.at(0);
    else if (f_contents.size() >= 2)
      message_line = f_contents.at(0) + ": " + f_contents.at(1);
    else
      return;

    if (lobby_constructed)
    {
      w_lobby->append_chat_message(message_line);
    }
    if (courtroom_constructed)
    {
      //T0D0: uncomment this when it's implemented
      //w_courtroom->append_chat_message(message_line);
    }
  }
}
