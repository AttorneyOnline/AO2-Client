#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "networkmanager.h"

void AOApplication::ms_packet_received(AOPacket *p_packet)
{
  QString header = p_packet->get_header();

  // Some packets need to handle decode/encode separately
  if (header != "SC") {
    p_packet->net_decode();
  }
  QStringList f_contents = p_packet->get_contents();

#ifdef DEBUG_NETWORK
  if (header != "CHECK")
    qDebug() << "R(ms):" << p_packet->to_string();
#endif

  if (header == "ALL") {
    server_list.clear();

    for (QString i_string : p_packet->get_contents()) {
      server_type f_server;
      QStringList sub_contents = i_string.split("&");

      if (sub_contents.size() < 4) {
        qDebug() << "W: malformed packet";
        continue;
      }

      f_server.name = sub_contents.at(0);
      f_server.desc = sub_contents.at(1);
      f_server.ip = sub_contents.at(2);
      f_server.port = sub_contents.at(3).toInt();

      server_list.append(f_server);
    }

    if (lobby_constructed) {
      w_lobby->list_servers();
    }
  }
  else if (header == "CT") {
    QString f_name, f_message;

    if (f_contents.size() == 1) {
      f_name = "";
      f_message = f_contents.at(0);
    }
    else if (f_contents.size() >= 2) {
      f_name = f_contents.at(0);
      f_message = f_contents.at(1);
    }
    else
      goto end;

    if (lobby_constructed) {
      w_lobby->append_chatmessage(f_name, f_message);
    }
    if (courtroom_constructed && courtroom_loaded) {
      w_courtroom->append_ms_chatmessage(f_name, f_message);
    }
  }
  else if (header == "AO2CHECK") {
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
    else if (get_release() == f_release) {
      if (get_major_version() > f_major)
        goto end;
      else if (get_major_version() == f_major) {
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

end:

  delete p_packet;
}

void AOApplication::append_to_demofile(QString packet_string)
{
    if (get_auto_logging_enabled() && !log_filename.isEmpty())
    {
        QString path = log_filename.left(log_filename.size()).replace(".log", ".demo");
        append_to_file(packet_string, path, true);
        if (!demo_timer.isValid())
            demo_timer.start();
        else
            append_to_file("wait#"+ QString::number(demo_timer.restart()) + "#%", path, true);
    }
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

  if (header == "decryptor") {
    if (f_contents.size() == 0)
      goto end;

    // default(legacy) values
    yellow_text_enabled = false;
    prezoom_enabled = false;
    flipping_enabled = false;
    custom_objection_enabled = false;
    desk_mod_enabled = false;
    evidence_enabled = false;
    cccc_ic_support_enabled = false;
    arup_enabled = false;
    casing_alerts_enabled = false;
    modcall_reason_enabled = false;
    looping_sfx_support_enabled = false;
    additive_enabled = false;
    effects_enabled = false;
    y_offset_enabled = false;

    QString f_hdid;
    f_hdid = get_hdid();

    AOPacket *hi_packet = new AOPacket("HI#" + f_hdid + "#%");
    send_server_packet(hi_packet);
  }
  else if (header == "ID") {
    if (f_contents.size() < 2)
      goto end;

    s_pv = f_contents.at(0).toInt();
    server_software = f_contents.at(1);

    if (lobby_constructed)
      w_lobby->enable_connect_button();

    send_server_packet(new AOPacket("ID#AO2#" + get_version_string() + "#%"));
  }
  else if (header == "CT") {
    if (f_contents.size() < 2)
      goto end;

    if (courtroom_constructed) {
      if (f_contents.size() == 3)
        w_courtroom->append_server_chatmessage(
            f_contents.at(0), f_contents.at(1), f_contents.at(2));
      else
        w_courtroom->append_server_chatmessage(f_contents.at(0),
                                               f_contents.at(1), "0");

      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "FL") {
    yellow_text_enabled = false;
    prezoom_enabled = false;
    flipping_enabled = false;
    custom_objection_enabled = false;
    desk_mod_enabled = false;
    evidence_enabled = false;
    cccc_ic_support_enabled = false;
    arup_enabled = false;
    casing_alerts_enabled = false;
    modcall_reason_enabled = false;
    looping_sfx_support_enabled = false;
    additive_enabled = false;
    effects_enabled = false;
    expanded_desk_mods_enabled = false;
    if (f_packet.contains("yellowtext", Qt::CaseInsensitive))
      yellow_text_enabled = true;
    if (f_packet.contains("prezoom", Qt::CaseInsensitive))
      prezoom_enabled = true;
    if (f_packet.contains("flipping", Qt::CaseInsensitive))
      flipping_enabled = true;
    if (f_packet.contains("customobjections", Qt::CaseInsensitive))
      custom_objection_enabled = true;
    if (f_packet.contains("deskmod", Qt::CaseInsensitive))
      desk_mod_enabled = true;
    if (f_packet.contains("evidence", Qt::CaseInsensitive))
      evidence_enabled = true;
    if (f_packet.contains("cccc_ic_support", Qt::CaseInsensitive))
      cccc_ic_support_enabled = true;
    if (f_packet.contains("arup", Qt::CaseInsensitive))
      arup_enabled = true;
    if (f_packet.contains("casing_alerts", Qt::CaseInsensitive))
      casing_alerts_enabled = true;
    if (f_packet.contains("modcall_reason", Qt::CaseInsensitive))
      modcall_reason_enabled = true;
    if (f_packet.contains("looping_sfx", Qt::CaseInsensitive))
      looping_sfx_support_enabled = true;
    if (f_packet.contains("additive", Qt::CaseInsensitive))
      additive_enabled = true;
    if (f_packet.contains("effects", Qt::CaseInsensitive))
      effects_enabled = true;
    if (f_packet.contains("y_offset", Qt::CaseInsensitive))
        y_offset_enabled = true;
    if (f_packet.contains("expanded_desk_mods", Qt::CaseInsensitive))
      expanded_desk_mods_enabled = true;
  }
  else if (header == "PN") {
    if (f_contents.size() < 2)
      goto end;

    w_lobby->set_player_count(f_contents.at(0).toInt(),
                              f_contents.at(1).toInt());

    if (w_lobby->doubleclicked) {
        send_server_packet(new AOPacket("askchaa#%"));
        w_lobby->doubleclicked = false;
    }
  }
  else if (header == "SI") {
    if (f_contents.size() != 3)
      goto end;

    char_list_size = f_contents.at(0).toInt();
    evidence_list_size = f_contents.at(1).toInt();
    music_list_size = f_contents.at(2).toInt();

    if (char_list_size < 0 || evidence_list_size < 0 || music_list_size < 0)
      goto end;

    loaded_chars = 0;
    loaded_evidence = 0;
    loaded_music = 0;
    generated_chars = 0;

    destruct_courtroom();
    construct_courtroom();

    courtroom_loaded = false;

    window_title = tr("Attorney Online 2");
    int selected_server = w_lobby->get_selected_server();

    QString server_address = "", server_name = "";
    if (w_lobby->public_servers_selected) {
      if (selected_server >= 0 && selected_server < server_list.size()) {
        auto info = server_list.at(selected_server);
        server_name = info.name;
        server_address =
            QString("%1:%2").arg(info.ip, QString::number(info.port));
        window_title += ": " + server_name;
      }
    }
    else {
      if (selected_server >= 0 && selected_server < favorite_list.size()) {
        auto info = favorite_list.at(selected_server);
        server_name = info.name;
        server_address =
            QString("%1:%2").arg(info.ip, QString::number(info.port));
        window_title += ": " + server_name;
      }
    }

    w_courtroom->set_window_title(window_title);

    w_lobby->show_loading_overlay();
    w_lobby->set_loading_text(tr("Loading"));
    w_lobby->set_loading_value(0);

    AOPacket *f_packet;

    f_packet = new AOPacket("RC#%");
    send_server_packet(f_packet);

    // Remove any characters not accepted in folder names for the server_name
    // here
    if (AOApplication::get_auto_logging_enabled() && server_name != "Demo playback") {
      this->log_filename = QDateTime::currentDateTime().toUTC().toString(
          "'logs/" + server_name.remove(QRegExp("[\\\\/:*?\"<>|\']")) +
          "/'yyyy-MM-dd hh-mm-ss t'.log'");
      this->write_to_file("Joined server " + server_name + " on address " +
                              server_address + " on " +
                              QDateTime::currentDateTime().toUTC().toString(),
                          log_filename, true);
    }
    else
      this->log_filename = "";

    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(server_address.toUtf8());
    if (is_discord_enabled())
      discord->state_server(server_name.toStdString(),
                            hash.result().toBase64().toStdString());
  }
  else if (header == "CharsCheck") {
    if (!courtroom_constructed)
      goto end;

    for (int n_char = 0; n_char < f_contents.size(); ++n_char) {
      if (f_contents.at(n_char) == "-1")
        w_courtroom->set_taken(n_char, true);
      else
        w_courtroom->set_taken(n_char, false);
    }
  }

  else if (header == "SC") {
    if (!courtroom_constructed || courtroom_loaded)
      goto end;

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      QStringList sub_elements = f_contents.at(n_element).split("&");

      AOPacket::unescape(sub_elements);

      char_type f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
        f_char.description = sub_elements.at(1);

      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++loaded_chars;

      w_lobby->set_loading_text(tr("Loading chars:\n%1/%2")
                                    .arg(QString::number(loaded_chars))
                                    .arg(QString::number(char_list_size)));

      w_courtroom->append_char(f_char);

      int total_loading_size =
          char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(
          ((loaded_chars + generated_chars + loaded_music + loaded_evidence) /
           static_cast<double>(total_loading_size)) *
          100);
      w_lobby->set_loading_value(loading_value);
    }

    send_server_packet(new AOPacket("RM#%"));
    append_to_demofile(p_packet->to_string(true));
  }
  else if (header == "SM") {
    if (!courtroom_constructed || courtroom_loaded)
      goto end;

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      ++loaded_music;

      w_lobby->set_loading_text(tr("Loading music:\n%1/%2")
                                    .arg(QString::number(loaded_music))
                                    .arg(QString::number(music_list_size)));

      if (musics_time) {
        w_courtroom->append_music(f_contents.at(n_element));
      }
      else {
        if (f_contents.at(n_element).endsWith(".wav") ||
            f_contents.at(n_element).endsWith(".mp3") ||
            f_contents.at(n_element).endsWith(".mp4") ||
            f_contents.at(n_element).endsWith(".ogg") ||
            f_contents.at(n_element).endsWith(".opus")) {
          musics_time = true;
          w_courtroom->fix_last_area();
          w_courtroom->append_music(f_contents.at(n_element));
          areas--;
        }
        else {
          w_courtroom->append_area(f_contents.at(n_element));
          areas++;
        }
      }

      for (int area_n = 0; area_n < areas; area_n++) {
        w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
      }

      int total_loading_size =
          char_list_size * 2 + evidence_list_size + music_list_size;
      int loading_value = int(
          ((loaded_chars + generated_chars + loaded_music + loaded_evidence) /
           static_cast<double>(total_loading_size)) *
          100);
      w_lobby->set_loading_value(loading_value);
    }

    send_server_packet(new AOPacket("RD#%"));
  }
  else if (header == "FM") // Fetch music ONLY
  {
    if (!courtroom_constructed)
      goto end;

    w_courtroom->clear_music();

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      w_courtroom->append_music(f_contents.at(n_element));
    }

    w_courtroom->list_music();
  }
  else if (header == "FA") // Fetch areas ONLY
  {
    if (!courtroom_constructed)
      goto end;

    w_courtroom->clear_areas();
    w_courtroom->arup_clear();

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      w_courtroom->append_area(f_contents.at(n_element));
      w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
    }

    w_courtroom->list_areas();
  }
  else if (header == "DONE") {
    if (!courtroom_constructed)
      goto end;

    if (lobby_constructed)
      w_courtroom->append_ms_chatmessage("", w_lobby->get_chatlog());

    w_courtroom->character_loading_finished();
    w_courtroom->done_received();

    courtroom_loaded = true;

    destruct_lobby();
  }
  else if (header == "BN") {
    if (f_contents.size() < 1)
      goto end;

    if (courtroom_constructed) {
      if (f_contents.size() >=
          2) // We have a pos included in the background packet!
        w_courtroom->set_side(f_contents.at(1));
      w_courtroom->set_background(f_contents.at(0), f_contents.size() >= 2);
      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "SP") {
    if (f_contents.size() < 1)
      goto end;

    if (courtroom_constructed) // We were sent a "set position" packet
    {
      w_courtroom->set_side(f_contents.at(0), false);
      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "SD") // Send pos dropdown
  {
    if (f_contents.size() < 1)
      goto end;

    w_courtroom->set_pos_dropdown(f_contents.at(0).split("*"));
  }
  // server accepting char request(CC) packet
  else if (header == "PV") {
    if (f_contents.size() < 3)
      goto end;

    w_courtroom->enter_courtroom();

    if (courtroom_constructed)
      w_courtroom->update_character(f_contents.at(2).toInt());
  }
  else if (header == "MS") {
    if (courtroom_constructed && courtroom_loaded)
    {
      w_courtroom->chatmessage_enqueue(p_packet->get_contents());
      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "MC") {
    if (courtroom_constructed && courtroom_loaded)
    {
      w_courtroom->handle_song(&p_packet->get_contents());
      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "RT") {
    if (f_contents.size() < 1)
      goto end;
    if (courtroom_constructed) {
        if (f_contents.size() == 1)
          w_courtroom->handle_wtce(f_contents.at(0), 0);
        else if (f_contents.size() == 2) {
          w_courtroom->handle_wtce(f_contents.at(0), f_contents.at(1).toInt());
        append_to_demofile(p_packet->to_string(true));
      }
    }
  }
  else if (header == "HP") {
    if (courtroom_constructed && f_contents.size() > 1)
    {
      w_courtroom->set_hp_bar(f_contents.at(0).toInt(),
                              f_contents.at(1).toInt());
      append_to_demofile(p_packet->to_string(true));
    }
  }
  else if (header == "LE") {
    if (courtroom_constructed) {
      QVector<evi_type> f_evi_list;

      for (QString f_string : f_contents) {
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
  else if (header == "ARUP") {
    if (courtroom_constructed) {
      int arup_type = f_contents.at(0).toInt();
      for (int n_element = 1; n_element < f_contents.size(); n_element++) {
        w_courtroom->arup_modify(arup_type, n_element - 1,
                                 f_contents.at(n_element));
      }
      w_courtroom->list_areas();
    }
  }
  else if (header == "IL") {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_ip_list(f_contents.at(0));
  }
  else if (header == "MU") {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_mute(true, f_contents.at(0).toInt());
  }
  else if (header == "UM") {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->set_mute(false, f_contents.at(0).toInt());
  }
  else if (header == "BB") {
    if (courtroom_constructed && f_contents.size() >= 1) {
      call_notice(f_contents.at(0));
    }
  }
  else if (header == "KK") {
    if (courtroom_constructed && f_contents.size() >= 1) {
      call_notice(tr("You have been kicked from the server.\nReason: %1")
                      .arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (header == "KB") {
    if (courtroom_constructed && f_contents.size() >= 1) {
      call_notice(tr("You have been banned from the server.\nReason: %1")
                      .arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (header == "BD") {
    call_notice(
        tr("You are banned on this server.\nReason: %1").arg(f_contents.at(0)));
  }
  else if (header == "ZZ") {
    if (courtroom_constructed && f_contents.size() > 0)
      w_courtroom->mod_called(f_contents.at(0));
  }
  else if (header == "CASEA") {
    if (courtroom_constructed && f_contents.size() > 6)
      w_courtroom->case_called(f_contents.at(0), f_contents.at(1) == "1",
                               f_contents.at(2) == "1", f_contents.at(3) == "1",
                               f_contents.at(4) == "1",
                               f_contents.at(5) == "1");
  }
  else if (header == "TI") { // Timer packet
    if (!courtroom_constructed || f_contents.size() < 2)
      goto end;

    // Timer ID is reserved as argument 0
    int id = f_contents.at(0).toInt();

    // Type 0 = start/resume/sync timer at time
    // Type 1 = pause timer at time
    // Type 2 = show timer
    // Type 3 = hide timer
    int type = f_contents.at(1).toInt();

    if (type == 0 || type == 1)
    {
      if (f_contents.size() < 2)
        goto end;

      // The time as displayed on the clock, in milliseconds.
      // If the number received is negative, stop the timer.
      qint64 timer_value = f_contents.at(2).toLongLong();
      qDebug() << "timer:" << timer_value;
      if (timer_value > 0)
      {
        if (type == 0)
        {
          timer_value -= latency / 2;
          w_courtroom->start_clock(id, timer_value);
        }
        else
        {
          w_courtroom->pause_clock(id);
          w_courtroom->set_clock(id, timer_value);
        }
      }
      else
      {
        w_courtroom->stop_clock(id);
      }
    }
    else if (type == 2)
      w_courtroom->set_clock_visibility(id, true);
    else if (type == 3)
      w_courtroom->set_clock_visibility(id, false);
  }
  else if (header == "CHECK") {
    if (!courtroom_constructed)
      goto end;

    qint64 ping_time = w_courtroom->pong();
    qDebug() << "ping:" << ping_time;
    if (ping_time != -1)
      latency = ping_time;
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

#ifdef DEBUG_NETWORK
    qDebug() << "S:" << f_packet;
#endif

  net_manager->ship_server_packet(f_packet);

  delete p_packet;
}
