#include "aoapplication.h"

#include "lobby.h"
#include "courtroom.h"
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
  }
  else if (header == "ID")
  {
    if (f_contents.size() < 1)
      return;

    s_pv = f_contents.at(0).toInt();

    if (f_contents.size() < 2)
      return;

    //T0D0: store server version
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
  else if (header == "SI")
  {
    if (f_contents.size() < 3)
      return;

    char_list_size = f_contents.at(0).toInt();
    loaded_chars = 0;
    evidence_list_size = f_contents.at(1).toInt();
    loaded_evidence = 0;
    music_list_size = f_contents.at(2).toInt();
    loaded_music = 0;

    destruct_courtroom();

    construct_courtroom();

    AOPacket *f_packet = new AOPacket("askchar2#%");
    send_server_packet(f_packet);
  }
  else if (header == "CI")
  {
    if (!courtroom_constructed)
      return;

    for (int n_element = 0 ; n_element < f_contents.size() ; n_element += 2)
    {
      if (f_contents.at(n_element).toInt() != loaded_chars)
        break;

      if (n_element == f_contents.size() - 1)
        break;

      QStringList sub_elements = f_contents.at(n_element + 1).split("&");
      if (sub_elements.size() < 2)
        break;

      char_type f_char;
      f_char.name = sub_elements.at(0);
      f_char.description = sub_elements.at(1);

      ++loaded_chars;
      qDebug() << "loaded_chars" << loaded_chars;

      w_courtroom->append_char(f_char);


      qDebug() << f_contents.at(n_element + 1);
    }

    if (loaded_chars < char_list_size)
    {
      qDebug() << "loaded_chars" << loaded_chars;
      QString next_packet_number = QString::number(((loaded_chars - 1) / 10) + 1);
      AOPacket *f_packet = new AOPacket("AN#" + next_packet_number + "#%");
      send_server_packet(f_packet);
    }



  }
}

void AOApplication::send_ms_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  net_manager->ship_ms_packet(f_packet);

  qDebug() << "S(ms):" << f_packet;

  delete p_packet;
}

void AOApplication::send_server_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  if (encryption_needed)
  {
    qDebug() << "S(e):" << f_packet;

    p_packet->encrypt_header(s_decryptor);
    f_packet = p_packet->to_string();
  }
  else
  {
    qDebug() << "S:" << f_packet;
  }

  net_manager->ship_server_packet(f_packet);

  delete p_packet;
}
