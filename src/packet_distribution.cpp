#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "networkmanager.h"

void AOApplication::append_to_demofile(QString packet_string)
{
    if (get_demo_logging_enabled() && !log_filename.isEmpty())
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
  if (packetMap.contains(header)) {
      packetMap.value(header)->handler(this, p_packet);
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
  else if (header == "CASEA") {
    if (courtroom_constructed && f_contents.size() >= 6)
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
    QString p_st = configini->value("subtheme").value<QString>();
    if (p_st.toLower() != "server")
      // We don't. Simply acknowledge the subtheme sent by the server, but don't do anything else.
      return;

    // Reload theme request
    if (f_contents.size() > 1 && f_contents.at(1) == "1")
      w_courtroom->on_reload_theme_clicked();
  }
  // Auth packet
  else if (header == "AUTH") {
    if (!courtroom_constructed || !auth_packet_supported || f_contents.isEmpty()) {
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
