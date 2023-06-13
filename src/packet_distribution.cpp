#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "networkmanager.h"
#include "options.h"

void AOApplication::append_to_demofile(QString packet_string)
{
    if (Options::getInstance().logToDemoFileEnabled() && !log_filename.isEmpty())
    {
        QString path = log_filename.left(log_filename.size()).replace(".log", ".demo");
        if (!demo_timer.isValid())
            demo_timer.start();
        else
            append_to_file("wait#"+ QString::number(demo_timer.restart()) + "#%", path, true);
        append_to_file(packet_string, path, true);
    }
}

void AOApplication::server_packet_received(AOPacket *p_packet)
{
  QStringList f_contents_encoded = p_packet->get_contents();
  QString f_packet_encoded = p_packet->to_string();
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();
  QString f_packet = p_packet->to_string();

  bool log_to_demo = true;

#ifdef DEBUG_NETWORK
  if (header != "checkconnection")
    qDebug() << "R:" << f_packet;
#endif

  if (header == "decryptor") {
    if (f_contents.size() == 0)
      goto end;

    QString f_hdid;
    f_hdid = get_hdid();

    QStringList f_contents = {f_hdid};
    AOPacket *hi_packet = new AOPacket("HI", f_contents);
    send_server_packet(hi_packet);
    log_to_demo = false;
  }
  else if (header == "ID") {
    if (f_contents.size() < 2)
      goto end;

    client_id = f_contents.at(0).toInt();
    server_software = f_contents.at(1);

    send_server_packet(new AOPacket("ID#misairu#" + get_version_string() + "#%"));
  }
  else if (header == "CT") {
    if (!courtroom_constructed || f_contents.size() < 2) {
      goto end;
}

    if (f_contents.size() == 3)
      w_courtroom->append_server_chatmessage(
          f_contents.at(0), f_contents.at(1), f_contents.at(2));
    else
      w_courtroom->append_server_chatmessage(f_contents.at(0),
                                             f_contents.at(1), "0");
  }
  else if (header == "PN") {
    if (!lobby_constructed || f_contents.size() < 2)
      goto end;
  }
  else if (header == "SI") {
    if (!lobby_constructed || f_contents.size() != 3) {
      goto end;
}

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

    w_courtroom->set_window_title("misairu alpha - not for distribution");

    w_lobby->show_loading_overlay();
    w_lobby->set_loading_text(tr("Loading"));
    w_lobby->set_loading_value(0);

    AOPacket *f_packet;

    f_packet = new AOPacket("RC");
    send_server_packet(f_packet);

    // Remove any characters not accepted in folder names for the server_name
    // here
    if (Options::getInstance().logToDemoFileEnabled() && server_name != "Demo playback") {
      this->log_filename = QDateTime::currentDateTime().toUTC().toString(
          "'logs/" + server_name.remove(QRegularExpression("[\\\\/:*?\"<>|\']")) +
          "/'yyyy-MM-dd hh-mm-ss t'.log'");
      this->write_to_file("Joined server " + server_name + " hosted on address " +
                              server_address + " on " +
                              QDateTime::currentDateTime().toUTC().toString(),
                          log_filename, true);
    }
    else
      this->log_filename = "";

    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(server_address.toUtf8());
    if (Options::getInstance().discordEnabled())
      discord->state_server(server_name.toStdString(),
                            hash.result().toBase64().toStdString());
    log_to_demo = false;
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
    log_to_demo = false;
  }

  else if (header == "SC") {
    if (!courtroom_constructed)
      goto end;
    w_courtroom->clear_chars();
    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      QStringList sub_elements = f_contents.at(n_element).split("&");

      AOPacket::unescape(sub_elements);

      char_type f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
        f_char.description = sub_elements.at(1);

      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      w_courtroom->append_char(f_char);
    }

    if (!courtroom_loaded)
      send_server_packet(new AOPacket("RM"));
    else
      w_courtroom->character_loading_finished();
  }
  else if (header == "SM") {
    if (!courtroom_constructed || courtroom_loaded)
      goto end;

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
      ++loaded_music;
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
    }

    for (int area_n = 0; area_n < areas; area_n++) {
      w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
    }

    send_server_packet(new AOPacket("RD"));
    log_to_demo = false;
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
    log_to_demo = false;
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
    log_to_demo = false;
  }
  else if (header == "DONE") {
    if (!courtroom_constructed)
      goto end;

    w_courtroom->character_loading_finished();
    w_courtroom->done_received();

    courtroom_loaded = true;

    destruct_lobby();
    log_to_demo = false;
  }
  else if (header == "BN") {
    if (!courtroom_constructed || f_contents.isEmpty())
      goto end;

    if (f_contents.size() >= 2) {
      // We have a pos included in the background packet!
        if (!f_contents.at(1).isEmpty())
          //Not touching it when its empty.
          w_courtroom->set_side(f_contents.at(1));
    }
    w_courtroom->set_background(f_contents.at(0), f_contents.size() >= 2);
  }
  else if (header == "SP") {
    if (!courtroom_constructed || f_contents.isEmpty())
      goto end;

    // We were sent a "set position" packet
    w_courtroom->set_side(f_contents.at(0));
  }
  else if (header == "SD") // Send pos dropdown
  {
    if (!courtroom_constructed || f_contents.isEmpty())
      goto end;

    w_courtroom->set_pos_dropdown(f_contents.at(0).split("*"));
  }
  // server accepting char request(CC) packet
  else if (header == "PV") {
    if (!courtroom_constructed || f_contents.size() < 3)
      goto end;
    // For some reason, args 0 and 1 are not used (from tsu3 they're client ID and a string "CID")
    w_courtroom->enter_courtroom();
    w_courtroom->set_courtroom_size();
    w_courtroom->update_character(f_contents.at(2).toInt());
  }
  else if (header == "MS") {
    if (courtroom_constructed && courtroom_loaded)
    {
      w_courtroom->chatmessage_enqueue(p_packet->get_contents());
    }
  }
  else if (header == "MC") {
    if (courtroom_constructed && courtroom_loaded)
    {
      w_courtroom->handle_song(&p_packet->get_contents());
    }
  }
  else if (header == "RT") {
    if (f_contents.isEmpty())
      goto end;
    if (courtroom_constructed) {
      if (f_contents.size() == 1)
        w_courtroom->handle_wtce(f_contents.at(0), 0);
      else if (f_contents.size() >= 2)
        w_courtroom->handle_wtce(f_contents.at(0), f_contents.at(1).toInt());
    }
  }
  else if (header == "HP") {
    if (courtroom_constructed && f_contents.size() >= 2)
    {
      w_courtroom->set_hp_bar(f_contents.at(0).toInt(),
                              f_contents.at(1).toInt());
    }
  }
  else if (header == "LE") {
    if (courtroom_constructed) {
      QVector<evi_type> f_evi_list;

      for (QString f_string : f_contents_encoded) {
        QStringList sub_contents = f_string.split("&");

        if (sub_contents.size() < 3)
          continue;

        // decoding has to be done here instead of on reception
        // because this packet uses & as a delimiter for some reason
        AOPacket::unescape(sub_contents);

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
    if (courtroom_constructed && !f_contents.isEmpty()) {
      int arup_type = f_contents.at(0).toInt();
      for (int n_element = 1; n_element < f_contents.size(); n_element++) {
        w_courtroom->arup_modify(arup_type, n_element - 1,
                                 f_contents.at(n_element));
      }
      w_courtroom->list_areas();
    }
    log_to_demo = false;
  }
  else if (header == "IL") {
    if (courtroom_constructed && !f_contents.isEmpty())
      w_courtroom->set_ip_list(f_contents.at(0));
    log_to_demo = false;
  }
  else if (header == "MU") {
    if (courtroom_constructed && !f_contents.isEmpty())
      w_courtroom->set_mute(true, f_contents.at(0).toInt());
    log_to_demo = false;
  }
  else if (header == "UM") {
    if (courtroom_constructed && !f_contents.isEmpty()) {
      w_courtroom->set_mute(false, f_contents.at(0).toInt());
    log_to_demo = false;
}
  }
  else if (header == "BB") {
    if (courtroom_constructed && !f_contents.isEmpty()) {
      call_notice(f_contents.at(0));
    }
    log_to_demo = false;
  }
  else if (header == "KK") {
    if (courtroom_constructed && !f_contents.isEmpty()) {
      call_notice(tr("You have been kicked from the server.\nReason: %1")
                      .arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
    log_to_demo = false;
  }
  else if (header == "KB") {
    if (courtroom_constructed && !f_contents.isEmpty()) {
      call_notice(tr("You have been banned from the server.\nReason: %1")
                      .arg(f_contents.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
    log_to_demo = false;
  }
  else if (header == "BD") {
    if (f_contents.isEmpty()) {
      goto end;
    }
    call_notice(
        tr("You are banned on this server.\nReason: %1").arg(f_contents.at(0)));
    log_to_demo = false;
  }
  else if (header == "ZZ") {
    if (courtroom_constructed && !f_contents.isEmpty())
      w_courtroom->mod_called(f_contents.at(0));
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
      if (f_contents.size() < 3)
        goto end;

      // The time as displayed on the clock, in milliseconds.
      // If the number received is negative, stop the timer.
      qint64 timer_value = f_contents.at(2).toLongLong();
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
    log_to_demo = false;
  }
  // Subtheme packet
  else if (header == "ST") {
    if (!courtroom_constructed || f_contents.isEmpty())
      goto end;
    // Subtheme reserved as argument 0
    subtheme = f_contents.at(0);

    // Check if we have subthemes set to "server"
    if (Options::getInstance().settingsSubTheme().toLower() != "server")
      // We don't. Simply acknowledge the subtheme sent by the server, but don't do anything else.
      return;

    // Reload theme request
    if (f_contents.size() > 1 && f_contents.at(1) == "1") {
      Options::getInstance().setServerSubTheme(subtheme);
      w_courtroom->on_reload_theme_clicked();
    }
  }
  // Auth packet
  else if (header == "AUTH") {
    if (!courtroom_constructed || f_contents.size() < 1)
      goto end;
}
    bool ok;
    int authenticated = f_contents.at(0).toInt(&ok);
    if (!ok) {
      qWarning() << "Malformed AUTH packet! Contents:" << f_contents.at(0);
    }

    w_courtroom->on_authentication_state_received(authenticated);
    log_to_demo = false;
  }
  else if (header == "JD") {
    if (!courtroom_constructed || f_contents.isEmpty()) {
      goto end;
    }
    bool ok;
    Courtroom::JudgeState state = static_cast<Courtroom::JudgeState>(f_contents.at(0).toInt(&ok));
    if (!ok) {
      goto end; // ignore malformed packet
    }
    w_courtroom->set_judge_state(state);
    if (w_courtroom->get_judge_state() != Courtroom::POS_DEPENDENT) { // If we receive JD -1, it means the server asks us to fall back to client-side judge buttons behavior
      w_courtroom->show_judge_controls(w_courtroom->get_judge_state() == Courtroom::SHOW_CONTROLS);
    }
    else {
      w_courtroom->set_judge_buttons(); // client-side judge behavior
    }
  }

 //AssetURL Packet
  else if (header == "ASS") {
    if (f_contents.size() > 1 || f_contents.isEmpty()) { // This can never be more than one link.
      goto end;
    }
    QUrl t_asset_url = QUrl::fromPercentEncoding(f_contents.at(0).toUtf8());
    if (t_asset_url.isValid())
    asset_url = t_asset_url.toString();
  }

  if (log_to_demo) {
    append_to_demofile(f_packet_encoded);
  }

end:

  delete p_packet;
}

void AOApplication::send_server_packet(AOPacket *p_packet)
{
  // ***NEVER*** send an unencoded packet.
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();
#ifdef DEBUG_NETWORK
    qDebug() << "S:" << f_packet;
#endif
  net_manager->ship_server_packet(f_packet);

  delete p_packet;
}
