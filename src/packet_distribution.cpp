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
    {
      demo_timer.start();
    }
    else
    {
      append_to_file("wait#" + QString::number(demo_timer.restart()) + "#%", path, true);
    }
    append_to_file(packet_string, path, true);
  }
}

void AOApplication::server_packet_received(AOPacket packet)
{
  QString header = packet.header();
  QStringList content = packet.content();

  bool log_to_demo = true;

#ifdef DEBUG_NETWORK
  if (header != "checkconnection")
  {
    qDebug() << "R:" << f_packet;
  }
#endif

  if (header == "decryptor")
  {
    if (content.size() == 0)
    {
      return;
    }

    // default(legacy) values
    m_serverdata.set_features(QStringList());

    QString f_hdid;
    f_hdid = get_hdid();

    QStringList f_contents = {f_hdid};
    AOPacket hi_packet("HI", f_contents);
    send_server_packet(hi_packet);
    log_to_demo = false;
  }
  else if (header == "ID")
  {
    if (content.size() < 2)
    {
      return;
    }

    client_id = content.at(0).toInt();
    m_serverdata.set_server_software(content.at(1));

    net_manager->server_connected(true);

    QStringList f_contents = {"AO2", get_version_string()};
    send_server_packet(AOPacket("ID", f_contents));
  }
  else if (header == "CT")
  {
    if (!is_courtroom_constructed() || content.size() < 2)
    {
      return;
    }

    if (content.size() == 3)
    {
      w_courtroom->append_server_chatmessage(content.at(0), content.at(1), content.at(2));
    }
    else
    {
      w_courtroom->append_server_chatmessage(content.at(0), content.at(1), "0");
    }
  }
  else if (header == "FL")
  {
    m_serverdata.set_features(content);
    w_courtroom->set_widgets();
    log_to_demo = false;
  }
  else if (header == "PN")
  {
    if (!is_lobby_constructed() || content.size() < 2)
    {
      return;
    }

    w_lobby->set_player_count(content.at(0).toInt(), content.at(1).toInt());

    if (content.size() >= 3)
    {
      w_lobby->set_server_description(content.at(2));
    }

    log_to_demo = false;
  }
  else if (header == "SI")
  {
    if (!is_lobby_constructed() || content.size() != 3)
    {
      return;
    }

    generated_chars = 0;

    int selected_server = w_lobby->get_selected_server();
    QString server_address;
    QString server_name;
    switch (w_lobby->pageSelected())
    {
    case 0:
      if (selected_server >= 0 && selected_server < server_list.size())
      {
        auto info = server_list.at(selected_server);
        server_name = info.name;
        server_address = QString("%1:%2").arg(info.address, QString::number(info.port));
        window_title = server_name;
      }
      break;

    case 1:
    {
      QVector<ServerInfo> favorite_list = Options::getInstance().favorites();
      if (selected_server >= 0 && selected_server < favorite_list.size())
      {
        auto info = favorite_list.at(selected_server);
        server_name = info.name;
        server_address = QString("%1:%2").arg(info.address, QString::number(info.port));
        window_title = server_name;
      }
    }
    break;
    case 2:
      window_title = "Local Demo Recording";
      break;
    default:
      break;
    }

    if (is_courtroom_constructed())
    {
      w_courtroom->set_window_title(window_title);
    }

    send_server_packet(AOPacket("RC"));

    // Remove any characters not accepted in folder names for the server_name
    // here
    if (Options::getInstance().logToDemoFileEnabled() && server_name != "Demo playback")
    {
      this->log_filename = QDateTime::currentDateTime().toUTC().toString("'logs/" + server_name.remove(QRegularExpression("[\\\\/:*?\"<>|\']")) + "/'yyyy-MM-dd hh-mm-ss t'.log'");
      this->write_to_file("Joined server " + server_name + " hosted on address " + server_address + " on " + QDateTime::currentDateTime().toUTC().toString(), log_filename, true);
    }
    else
    {
      this->log_filename = "";
    }

    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(server_address.toUtf8());
    if (Options::getInstance().discordEnabled())
    {
      discord->state_server(server_name.toStdString(), hash.result().toBase64().toStdString());
    }
    log_to_demo = false;
  }
  else if (header == "CharsCheck")
  {
    if (!is_courtroom_constructed())
    {
      return;
    }

    for (int n_char = 0; n_char < content.size(); ++n_char)
    {
      if (content.at(n_char) == "-1")
      {
        w_courtroom->set_taken(n_char, true);
      }
      else
      {
        w_courtroom->set_taken(n_char, false);
      }
    }
    log_to_demo = false;
  }

  else if (header == "SC")
  {
    if (!is_courtroom_constructed())
    {
      return;
    }
    w_courtroom->clear_chars();
    for (int n_element = 0; n_element < content.size(); ++n_element)
    {
      QStringList sub_elements = content.at(n_element).split("&");
      for (QString &sub_element : sub_elements)
      {
        sub_element = AOPacket::decode(sub_element);
      }

      CharacterSlot f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
      {
        f_char.description = sub_elements.at(1);
      }

      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      w_courtroom->append_char(f_char);
    }

    if (!courtroom_loaded)
    {
      send_server_packet(AOPacket("RM"));
    }
    else
    {
      w_courtroom->character_loading_finished();
    }
  }
  else if (header == "SM")
  {
    if (!is_courtroom_constructed() || courtroom_loaded)
    {
      return;
    }

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0; n_element < content.size(); ++n_element)
    {
      if (musics_time)
      {
        w_courtroom->append_music(content.at(n_element));
      }
      else
      {
        if (content.at(n_element).endsWith(".wav") || content.at(n_element).endsWith(".mp3") || content.at(n_element).endsWith(".mp4") || content.at(n_element).endsWith(".ogg") || content.at(n_element).endsWith(".opus"))
        {
          musics_time = true;
          w_courtroom->fix_last_area();
          w_courtroom->append_music(content.at(n_element));
          areas--;
        }
        else
        {
          w_courtroom->append_area(content.at(n_element));
          areas++;
        }
      }
    }

    for (int area_n = 0; area_n < areas; area_n++)
    {
      w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
    }

    send_server_packet(AOPacket("RD"));

    // TODO : Implement username messaging and requirement server-side properly.
    send_server_packet(AOPacket("CT", {Options::getInstance().username(), ""}));
    log_to_demo = false;
  }
  else if (header == "FM") // Fetch music ONLY
  {
    if (!is_courtroom_constructed())
    {
      return;
    }

    w_courtroom->clear_music();

    for (int n_element = 0; n_element < content.size(); ++n_element)
    {
      w_courtroom->append_music(content.at(n_element));
    }

    w_courtroom->list_music();
    log_to_demo = false;
  }
  else if (header == "FA") // Fetch areas ONLY
  {
    if (!is_courtroom_constructed())
    {
      return;
    }

    w_courtroom->clear_areas();
    w_courtroom->arup_clear();

    for (int n_element = 0; n_element < content.size(); ++n_element)
    {
      w_courtroom->append_area(content.at(n_element));
      w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
    }

    w_courtroom->list_areas();
    log_to_demo = false;
  }
  else if (header == "DONE")
  {
    if (!is_courtroom_constructed())
    {
      return;
    }

    w_courtroom->character_loading_finished();
    w_courtroom->done_received();

    courtroom_loaded = true;

    destruct_lobby();
    log_to_demo = false;
  }
  else if (header == "BN")
  {
    if (!is_courtroom_constructed() || content.isEmpty())
    {
      return;
    }

    if (content.size() >= 2)
    {
      // We have a pos included in the background packet!
      if (!content.at(1).isEmpty())
      {
        // Not touching it when its empty.
        w_courtroom->set_side(content.at(1));
      }
    }
    w_courtroom->set_background(content.at(0), content.size() >= 2);
  }
  else if (header == "SP")
  {
    if (!is_courtroom_constructed() || content.isEmpty())
    {
      return;
    }

    // We were sent a "set position" packet
    w_courtroom->set_side(content.at(0));
  }
  else if (header == "SD") // Send pos dropdown
  {
    if (!is_courtroom_constructed() || content.isEmpty())
    {
      return;
    }

    w_courtroom->set_pos_dropdown(content.at(0).split("*"));
  }
  // server accepting char request(CC) packet
  else if (header == "PV")
  {
    if (!is_courtroom_constructed() || content.size() < 3)
    {
      return;
    }
    // For some reason, args 0 and 1 are not used (from tsu3 they're client ID and a string "CID")
    w_courtroom->enter_courtroom();
    w_courtroom->set_courtroom_size();
    w_courtroom->update_character(content.at(2).toInt());
  }
  else if (header == "MS")
  {
    if (is_courtroom_constructed() && courtroom_loaded)
    {
      w_courtroom->chatmessage_enqueue(packet.content());
    }
  }
  else if (header == "MC")
  {
    if (is_courtroom_constructed() && courtroom_loaded)
    {
      w_courtroom->handle_song(&packet.content());
    }
  }
  else if (header == "RT")
  {
    if (content.isEmpty())
    {
      return;
    }
    if (is_courtroom_constructed())
    {
      if (content.size() == 1)
      {
        w_courtroom->handle_wtce(content.at(0), 0);
      }
      else if (content.size() >= 2)
      {
        w_courtroom->handle_wtce(content.at(0), content.at(1).toInt());
      }
    }
  }
  else if (header == "HP")
  {
    if (is_courtroom_constructed() && content.size() >= 2)
    {
      w_courtroom->set_hp_bar(content.at(0).toInt(), content.at(1).toInt());
    }
  }
  else if (header == "LE")
  {
    if (is_courtroom_constructed())
    {
      QVector<EvidenceItem> f_evi_list;

      for (QString f_string : packet.content())
      {
        QStringList sub_contents = f_string.split("&");
        if (sub_contents.size() < 3)
        {
          continue;
        }

        // decoding has to be done here instead of on reception
        // because this packet uses & as a delimiter for some reason
        for (QString &data : sub_contents)
        {
          data = AOPacket::decode(data);
        }

        EvidenceItem f_evi;
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
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      int arup_type = content.at(0).toInt();
      for (int n_element = 1; n_element < content.size(); n_element++)
      {
        w_courtroom->arup_modify(arup_type, n_element - 1, content.at(n_element));
      }
      w_courtroom->list_areas();
    }
    log_to_demo = false;
  }
  else if (header == "IL")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      w_courtroom->set_ip_list(content.at(0));
    }
    log_to_demo = false;
  }
  else if (header == "MU")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      w_courtroom->set_mute(true, content.at(0).toInt());
    }
    log_to_demo = false;
  }
  else if (header == "UM")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      w_courtroom->set_mute(false, content.at(0).toInt());
      log_to_demo = false;
    }
  }
  else if (header == "BB")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      call_notice(content.at(0));
    }
    log_to_demo = false;
  }
  else if (header == "KK")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      call_notice(tr("You have been kicked from the server.\nReason: %1").arg(content.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
    log_to_demo = false;
  }
  else if (header == "KB")
  {
    if (is_courtroom_constructed() && !content.isEmpty())
    {
      call_notice(tr("You have been banned from the server.\nReason: %1").arg(content.at(0)));
      construct_lobby();
      destruct_courtroom();
    }
    log_to_demo = false;
  }
  else if (header == "BD")
  {
    if (content.isEmpty())
    {
      return;
    }
    call_notice(tr("You are banned on this server.\nReason: %1").arg(content.at(0)));
    log_to_demo = false;
  }
  else if (header == "ZZ")
  {
    if (content.size() < 1)
    {
      return;
    }

    if (is_courtroom_constructed() && !content.isEmpty())
    {
      w_courtroom->mod_called(content.at(0));
    }
  }
  else if (header == "TI")
  { // Timer packet
    if (!is_courtroom_constructed() || content.size() < 2)
    {
      return;
    }

    // Timer ID is reserved as argument 0
    int id = content.at(0).toInt();

    // Type 0 = start/resume/sync timer at time
    // Type 1 = pause timer at time
    // Type 2 = show timer
    // Type 3 = hide timer
    int type = content.at(1).toInt();

    if (type == 0 || type == 1)
    {
      if (content.size() < 3)
      {
        return;
      }

      // The time as displayed on the clock, in milliseconds.
      // If the number received is negative, stop the timer.
      qint64 timer_value = content.at(2).toLongLong();
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
    {
      w_courtroom->set_clock_visibility(id, true);
    }
    else if (type == 3)
    {
      w_courtroom->set_clock_visibility(id, false);
    }
  }
  else if (header == "CHECK")
  {
    if (!is_courtroom_constructed())
    {
      return;
    }

    qint64 ping_time = w_courtroom->pong();
    qDebug() << "ping:" << ping_time;
    if (ping_time != -1)
    {
      latency = ping_time;
    }
    log_to_demo = false;
  }
  // Subtheme packet
  else if (header == "ST")
  {
    if (!is_courtroom_constructed() || content.isEmpty())
    {
      return;
    }
    // Subtheme reserved as argument 0
    subtheme = content.at(0);

    // Check if we have subthemes set to "server"
    if (Options::getInstance().settingsSubTheme().toLower() != "server")
    {
      // We don't. Simply acknowledge the subtheme sent by the server, but don't do anything else.
      return;
    }

    // Reload theme request
    if (content.size() > 1 && content.at(1) == "1")
    {
      Options::getInstance().setServerSubTheme(subtheme);
      w_courtroom->on_reload_theme_clicked();
    }
  }
  // Auth packet
  else if (header == "AUTH")
  {
    if (!is_courtroom_constructed() || !m_serverdata.get_feature(server::BASE_FEATURE_SET::AUTH_PACKET) || content.isEmpty())
    {
      return;
    }
    bool ok;
    int authenticated = content.at(0).toInt(&ok);
    if (!ok)
    {
      qWarning() << "Malformed AUTH packet! Contents:" << content.at(0);
    }

    w_courtroom->on_authentication_state_received(authenticated);
    log_to_demo = false;
  }
  else if (header == "JD")
  {
    if (!is_courtroom_constructed() || content.isEmpty())
    {
      return;
    }
    bool ok;
    Courtroom::JudgeState state = static_cast<Courtroom::JudgeState>(content.at(0).toInt(&ok));
    if (!ok)
    {
      return; // ignore malformed packet
    }
    w_courtroom->set_judge_state(state);
    if (w_courtroom->get_judge_state() != Courtroom::POS_DEPENDENT)
    { // If we receive JD -1, it means the server asks us to fall back to client-side judge buttons behavior
      w_courtroom->show_judge_controls(w_courtroom->get_judge_state() == Courtroom::SHOW_CONTROLS);
    }
    else
    {
      w_courtroom->set_judge_buttons(); // client-side judge behavior
    }
  }

  // AssetURL Packet
  else if (header == "ASS")
  {
    if (content.size() > 1 || content.isEmpty())
    { // This can never be more than one link.
      return;
    }

    m_serverdata.set_asset_url(content.at(0));
  }
  else if (header == "PR")
  {
    if (content.size() < 2 || !is_courtroom_constructed())
    {
      return;
    }

    PlayerRegister update{content.at(0).toInt(), PlayerRegister::REGISTER_TYPE(content.at(1).toInt())};
    w_courtroom->playerList()->registerPlayer(update);

    if (log_to_demo)
    {
      append_to_demofile(packet.toString(true));
    }
  }
  else if (header == "PU")
  {
    if (content.size() < 3 || !is_courtroom_constructed())
    {
      return;
    }

    PlayerUpdate update{content.at(0).toInt(), PlayerUpdate::DATA_TYPE(content.at(1).toInt()), content.at(2)};
    w_courtroom->playerList()->updatePlayer(update);
  }

  if (log_to_demo)
  {
    append_to_demofile(packet.toString(true));
  }
}

void AOApplication::send_server_packet(AOPacket p_packet)
{
  QString f_packet = p_packet.toString();
#ifdef DEBUG_NETWORK
  qDebug() << "S:" << p_packet.to_string();
#endif
  net_manager->ship_server_packet(p_packet);
}
