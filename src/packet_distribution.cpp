#include "aoapplication.h"

#include "lobby.h"
#include "courtroom.h"
#include "networkmanager.h"
#include "encryption_functions.h"
#include "hardware_functions.h"
#include "debug_functions.h"

void AOApplication::ms_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();

#ifdef DEBUG_NETWORK
  if (header != "CHECK")
    qDebug() << "R(ms):" << p_packet->to_string();
#endif

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
    send_ms_packet(new AOPacket("HI#" + get_hdid() + "#%"));

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

    call_notice(tr("Outdated version! Your version: %1\n"
                   "Please go to aceattorneyonline.com to update.")
                .arg(get_version_string()));
    destruct_courtroom();
    destruct_lobby();
  }
  else if (header == "DOOM")
  {
    call_notice(tr("You have been exiled from AO.\n"
                   "Have a nice day."));
    destruct_courtroom();
    destruct_lobby();
  }

  end:

  delete p_packet;
}

void AOApplication::server_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();
  QString f_packet = p_packet->to_string();

#ifdef DEBUG_NETWORK
  if (header != "checkconnection")
    qDebug() << "R:" << f_packet;
#endif

  if (header == "decryptor")
  {
    if (f_contents.size() == 0)
      goto end;

    //you may ask where 322 comes from. that would be a good question.
    s_decryptor = fanta_decrypt(f_contents.at(0), 322).toUInt();

    //default(legacy) values
    encryption_needed = true;
    yellow_text_enabled = false;
    prezoom_enabled = false;
    flipping_enabled = false;
    custom_objection_enabled = false;
    improved_loading_enabled = false;
    desk_mod_enabled = false;
    evidence_enabled = false;
    cccc_ic_support_enabled = false;
    arup_enabled = false;
    casing_alerts_enabled = false;
    modcall_reason_enabled = false;

    //workaround for tsuserver4
    if (f_contents.at(0) == "NOENCRYPT")
      encryption_needed = false;

    QString f_hdid;
    f_hdid = get_hdid();

    AOPacket *hi_packet = new AOPacket("HI#" + f_hdid + "#%");
    send_server_packet(hi_packet);
  }
  else if (header == "ID")
  {
    if (f_contents.size() < 2)
      goto end;

    s_pv = f_contents.at(0).toInt();
    server_software = f_contents.at(1);

    w_lobby->enable_connect_button();

    send_server_packet(new AOPacket("ID#AO2#" + get_version_string() + "#%"));
  }
  else if (header == "CT")
  {
    if (f_contents.size() < 2)
      goto end;

    if (courtroom_constructed)
    {
      if (f_contents.size() == 3)
        w_courtroom->append_server_chatmessage(f_contents.at(0), f_contents.at(1), f_contents.at(2));
      else
        w_courtroom->append_server_chatmessage(f_contents.at(0), f_contents.at(1), "0");
    }
  }
  else if (header == "FL")
  {
    if (f_packet.contains("yellowtext",Qt::CaseInsensitive))
      yellow_text_enabled = true;
    if (f_packet.contains("flipping",Qt::CaseInsensitive))
      flipping_enabled = true;
    if (f_packet.contains("customobjections",Qt::CaseInsensitive))
      custom_objection_enabled = true;
    if (f_packet.contains("fastloading",Qt::CaseInsensitive))
      improved_loading_enabled = true;
    if (f_packet.contains("noencryption",Qt::CaseInsensitive))
      encryption_needed = false;
    if (f_packet.contains("deskmod",Qt::CaseInsensitive))
      desk_mod_enabled = true;
    if (f_packet.contains("evidence",Qt::CaseInsensitive))
      evidence_enabled = true;
    if (f_packet.contains("cccc_ic_support",Qt::CaseInsensitive))
      cccc_ic_support_enabled = true;
    if (f_packet.contains("arup",Qt::CaseInsensitive))
      arup_enabled = true;
    if (f_packet.contains("casing_alerts",Qt::CaseInsensitive))
      casing_alerts_enabled = true;
    if (f_packet.contains("modcall_reason",Qt::CaseInsensitive))
      modcall_reason_enabled = true;
  }
  else if (header == "PN")
  {
    if (f_contents.size() < 2)
      goto end;

    w_lobby->set_player_count(f_contents.at(0).toInt(), f_contents.at(1).toInt());
  }
  else if (header == "SI")
  {
    if (f_contents.size() != 3)
      goto end;

    char_list_size = f_contents.at(0).toInt();
    evidence_list_size = f_contents.at(1).toInt();
    music_list_size = f_contents.at(2).toInt();

    if (char_list_size < 1 || evidence_list_size < 0 || music_list_size < 0)
      goto end;

    loaded_chars = 0;
    loaded_evidence = 0;
    loaded_music = 0;
    generated_chars = 0;

    destruct_courtroom();
    construct_courtroom();

    courtroom_loaded = false;

    QString window_title = tr("Attorney Online 2");
    int selected_server = w_lobby->get_selected_server();

    QString server_address = "", server_name = "";
    if (w_lobby->public_servers_selected)
    {
      if (selected_server >= 0 && selected_server < server_list.size()) {
        auto info = server_list.at(selected_server);
        server_name = info.name;
        server_address = QString("%1:%2").arg(info.ip, info.port);
        window_title += ": " + server_name;
      }
    }
    else
    {
      if (selected_server >= 0 && selected_server < favorite_list.size()) {
        auto info = favorite_list.at(selected_server);
        server_name = info.name;
        server_address = info.ip + info.port;
        window_title += ": " + server_name;
      }
    }

    w_courtroom->set_window_title(window_title);

    w_lobby->show_loading_overlay();
    w_lobby->set_loading_text(tr("Loading"));
    w_lobby->set_loading_value(0);

    AOPacket *f_packet;

    if(improved_loading_enabled)
      f_packet = new AOPacket("RC#%");
    else
      f_packet = new AOPacket("askchar2#%");

    send_server_packet(f_packet);

    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(server_address.toUtf8());
    if (is_discord_enabled())
      discord->state_server(server_name.toStdString(), hash.result().toBase64().toStdString());
  }
  else if (header == "CI")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_element = 0 ; n_element < f_contents.size() ; n_element += 2)
    {
      if (f_contents.at(n_element).toInt() != loaded_chars)
        break;

      //this means we are on the last element and checking n + 1 element will be game over so
      if (n_element == f_contents.size() - 1)
        break;

      QStringList sub_elements = f_contents.at(n_element + 1).split("&");
      if (sub_elements.size() < 2)
        break;

      char_type f_char;
      f_char.name = sub_elements.at(0);
      f_char.description = sub_elements.at(1);
      f_char.evidence_string = sub_elements.at(3);
      //temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++loaded_chars;

      w_lobby->set_loading_text("Loading chars:\n" + QString::number(loaded_chars) + "/" + QString::number(char_list_size));

      w_courtroom->append_char(f_char);

      int total_loading_size = char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(((loaded_chars + generated_chars + loaded_music + loaded_evidence) / static_cast<double>(total_loading_size)) * 100);
      w_lobby->set_loading_value(loading_value);
    }

    if (improved_loading_enabled)
      send_server_packet(new AOPacket("RE#%"));
    else
    {
      QString next_packet_number = QString::number(((loaded_chars - 1) / 10) + 1);
      send_server_packet(new AOPacket("AN#" + next_packet_number + "#%"));
    }

  }
  else if (header == "EI")
  {
    if (!courtroom_constructed)
      goto end;


    // +1 because evidence starts at 1 rather than 0 for whatever reason
    //enjoy fanta
    if (f_contents.at(0).toInt() != loaded_evidence + 1)
      goto end;

    if (f_contents.size() < 2)
      goto end;

    QStringList sub_elements = f_contents.at(1).split("&");
    if (sub_elements.size() < 4)
      goto end;

    evi_type f_evi;
    f_evi.name = sub_elements.at(0);
    f_evi.description = sub_elements.at(1);
    //no idea what the number at position 2 is. probably an identifier?
    f_evi.image = sub_elements.at(3);

    ++loaded_evidence;

    w_lobby->set_loading_text(tr("Loading evidence:\n%1/%2").arg(QString::number(loaded_evidence)).arg(QString::number(evidence_list_size)));

    w_courtroom->append_evidence(f_evi);

    int total_loading_size = char_list_size * 2 + evidence_list_size + music_list_size;
    int loading_value = int(((loaded_chars + generated_chars + loaded_music + loaded_evidence) / static_cast<double>(total_loading_size)) * 100);
    w_lobby->set_loading_value(loading_value);

    QString next_packet_number = QString::number(loaded_evidence);
    send_server_packet(new AOPacket("AE#" + next_packet_number + "#%"));

  }
  else if (header == "EM")
  {
    if (!courtroom_constructed)
      goto end;

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0 ; n_element < f_contents.size() ; n_element += 2)
    {
      if (f_contents.at(n_element).toInt() != loaded_music)
        break;

      if (n_element == f_contents.size() - 1)
        break;

      QString f_music = f_contents.at(n_element + 1);

      ++loaded_music;

      w_lobby->set_loading_text(tr("Loading music:\n%1/%2").arg(QString::number(loaded_music)).arg(QString::number(music_list_size)));

      if (musics_time)
      {
          w_courtroom->append_music(f_music);
      }
      else
      {
          if (f_music.endsWith(".wav") ||
                  f_music.endsWith(".mp3") ||
                  f_music.endsWith(".mp4") ||
                  f_music.endsWith(".ogg") ||
                  f_music.endsWith(".opus"))
          {
              musics_time = true;
              areas--;
              w_courtroom->fix_last_area();
              w_courtroom->append_music(f_music);
          }
          else
          {
              w_courtroom->append_area(f_music);
              areas++;
          }
      }

      for (int area_n = 0; area_n < areas; area_n++)
      {
          w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
      }

      int total_loading_size = char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(((loaded_chars + generated_chars + loaded_music + loaded_evidence) / static_cast<double>(total_loading_size)) * 100);
      w_lobby->set_loading_value(loading_value);
    }

    QString next_packet_number = QString::number(((loaded_music - 1) / 10) + 1);
    send_server_packet(new AOPacket("AM#" + next_packet_number + "#%"));
  }
  else if (header == "CharsCheck")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_char = 0 ; n_char < f_contents.size() ; ++n_char)
    {
      if (f_contents.at(n_char) == "-1")
        w_courtroom->set_taken(n_char, true);
      else
        w_courtroom->set_taken(n_char, false);
    }
  }

  else if (header == "SC")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_element = 0 ; n_element < f_contents.size() ; ++n_element)
    {
      QStringList sub_elements = f_contents.at(n_element).split("&");

      char_type f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
        f_char.description = sub_elements.at(1);

      //temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++loaded_chars;

      w_lobby->set_loading_text(tr("Loading chars:\n%1/%2").arg(QString::number(loaded_chars)).arg(QString::number(char_list_size)));

      w_courtroom->append_char(f_char);

      int total_loading_size = char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(((loaded_chars + generated_chars + loaded_music + loaded_evidence) / static_cast<double>(total_loading_size)) * 100);
      w_lobby->set_loading_value(loading_value);
    }

    send_server_packet(new AOPacket("RM#%"));
  }
  else if (header == "SM")
  {
    if (!courtroom_constructed)
      goto end;

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0 ; n_element < f_contents.size() ; ++n_element)
    {
      ++loaded_music;

      w_lobby->set_loading_text(tr("Loading music:\n%1/%2").arg(QString::number(loaded_music)).arg(QString::number(music_list_size)));

      if (musics_time)
      {
          w_courtroom->append_music(f_contents.at(n_element));
      }
      else
      {
          if (f_contents.at(n_element).endsWith(".wav") ||
                  f_contents.at(n_element).endsWith(".mp3") ||
                  f_contents.at(n_element).endsWith(".mp4") ||
                  f_contents.at(n_element).endsWith(".ogg") ||
                  f_contents.at(n_element).endsWith(".opus"))
          {
              musics_time = true;
              w_courtroom->fix_last_area();
              w_courtroom->append_music(f_contents.at(n_element));
              areas--;
          }
          else
          {
              w_courtroom->append_area(f_contents.at(n_element));
              areas++;
          }
      }

      for (int area_n = 0; area_n < areas; area_n++)
      {
          w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
      }

      int total_loading_size = char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(((loaded_chars + generated_chars + loaded_music + loaded_evidence) / static_cast<double>(total_loading_size)) * 100);
      w_lobby->set_loading_value(loading_value);
    }

    send_server_packet(new AOPacket("RD#%"));
  }
  else if (header == "DONE")
  {
    if (!courtroom_constructed)
      goto end;

    if (lobby_constructed)
      w_courtroom->append_ms_chatmessage("", w_lobby->get_chatlog());

    w_courtroom->character_loading_finished();
    w_courtroom->done_received();

    courtroom_loaded = true;

    destruct_lobby();
  }
  else if (header == "BN")
  {
    if (f_contents.size() < 1)
      goto end;

    if (courtroom_constructed)
      w_courtroom->set_background(f_contents.at(0));
  }
  //server accepting char request(CC) packet
  else if (header == "PV")
  {
    if (f_contents.size() < 3)
      goto end;

    if (courtroom_constructed)
      w_courtroom->enter_courtroom(f_contents.at(2).toInt());
  }
  else if (header == "MS")
  {
    if (courtroom_constructed && courtroom_loaded)
      w_courtroom->handle_chatmessage(&p_packet->get_contents());
  }
  else if (header == "MC")
  {
    if (courtroom_constructed && courtroom_loaded)
      w_courtroom->handle_song(&p_packet->get_contents());
  }
  else if (header == "RT")
  {
    if (f_contents.size() < 1)
      goto end;
    if (courtroom_constructed)
    {
      if (f_contents.size() == 1)
          w_courtroom->handle_wtce(f_contents.at(0), 0);
      else if (f_contents.size() == 2) {
          w_courtroom->handle_wtce(f_contents.at(0), f_contents.at(1).toInt());
      }
    }
  }
  else if (header == "HP")
  {
    if (courtroom_constructed && f_contents.size() > 1)
      w_courtroom->set_hp_bar(f_contents.at(0).toInt(), f_contents.at(1).toInt());
  }
  else if (header == "LE")
  {
    if (courtroom_constructed)
    {
      QVector<evi_type> f_evi_list;

      for (QString f_string : f_contents)
      {
        QStringList sub_contents = f_string.split("&");

        if (sub_contents.size() < 3)
          continue;

        evi_type f_evi;
        f_evi.name = sub_contents.at(0);
        f_evi.description = sub_contents.at(1);
        f_evi.image = sub_contents.at(2);

        f_evi_list.append(f_evi);
      }

      w_courtroom->set_evidence_list(f_evi_list);
    }
  }
  else if (header == "ARUP")
  {
      if (courtroom_constructed)
      {
        int arup_type = f_contents.at(0).toInt();
        for (int n_element = 1 ; n_element < f_contents.size() ; n_element++)
        {
            w_courtroom->arup_modify(arup_type, n_element - 1, f_contents.at(n_element));
        }
      }
  }
  else if (header == "IL")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_ip_list(f_contents.at(0));
  }
  else if (header == "MU")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_mute(true, f_contents.at(0).toInt());
  }
  else if (header == "UM")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_mute(false, f_contents.at(0).toInt());
  }
  else if (header == "KK")
  {
    if (courtroom_constructed && f_contents.size() >= 1)
    {
      call_notice(tr("You have been kicked from the server.\nReason: %1").arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (header == "KB")
  {
    if (courtroom_constructed && f_contents.size() >= 1)
    {
      call_notice(tr("You have been banned from the server.\nReason: %1").arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }

  }
  else if (header == "BD")
  {
    call_notice(tr("You are banned on this server.\nReason: %1").arg(f_contents.at(0)));
  }
  else if (header == "ZZ")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->mod_called(f_contents.at(0));
  }
  else if (header == "CASEA")
  {
    if (courtroom_constructed && f_contents.size() > 6)
      w_courtroom->case_called(f_contents.at(0), f_contents.at(1) == "1", f_contents.at(2) == "1", f_contents.at(3) == "1", f_contents.at(4) == "1", f_contents.at(5) == "1");
  }

  end:

  delete p_packet;
}

void AOApplication::send_ms_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  net_manager->ship_ms_packet(f_packet);

#ifdef DEBUG_NETWORK
  qDebug() << "S(ms):" << f_packet;
#endif

  delete p_packet;
}

void AOApplication::send_server_packet(AOPacket *p_packet, bool encoded)
{
  if (encoded)
    p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  if (encryption_needed)
  {
#ifdef DEBUG_NETWORK
    qDebug() << "S(e):" << f_packet;
#endif

    p_packet->encrypt_header(s_decryptor);
    f_packet = p_packet->to_string();
  }
  else
  {
#ifdef DEBUG_NETWORK
    qDebug() << "S:" << f_packet;
#endif
  }

  net_manager->ship_server_packet(f_packet);

  delete p_packet;
}
