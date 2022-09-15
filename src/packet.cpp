#include "packet.h"
#include "aoapplication.h"
#include "aopacket.h"
#include "courtroom.h"
#include "hardware_functions.h"

void Decryptor::handler(AOApplication* app, AOPacket* packet){
    if (packet->to_string().size() == 0) {
      delete packet;
      return;
    }

    // default(legacy) values
    app->yellow_text_supported = false;
    app->prezoom_supported = false;
    app->flipping_supported = false;
    app->custom_objection_supported = false;
    app->desk_mod_supported = false;
    app->evidence_supported = false;
    app->cccc_ic_supported = false;
    app->arup_supported = false;
    app->casing_alerts_supported = false;
    app->modcall_reason_supported = false;
    app->looping_sfx_supported = false;
    app->additive_text_supported = false;
    app->effects_supported = false;
    app->y_offset_supported = false;

    QString f_hdid;

    f_hdid = get_hdid();

    QStringList f_contents = {f_hdid};
    AOPacket *hi_packet = new AOPacket("HI", f_contents);
    app->send_server_packet(hi_packet);
    logToDemo = false;
}

void MS::handler(AOApplication* app, AOPacket* packet) {
    if (app->courtroom_constructed && app->courtroom_loaded)
    {
      app->w_courtroom->chatmessage_enqueue(packet->get_contents());
    }
}

void ID::handler(AOApplication* app, AOPacket* packet) {
      if (packet->to_string().size() < 2)
        return;

      app->client_id = packet->get_contents().at(0).toInt();
      app->server_software = packet->get_contents().at(1);

      if (app->lobby_constructed)
        app->w_lobby->enable_connect_button();

      QStringList f_contents = {"AO2", app->get_version_string()};
      app->send_server_packet(new AOPacket("ID", f_contents));
}

void CharsCheck::handler(AOApplication* app, AOPacket* packet) {
    if (!app->courtroom_constructed) {
      delete packet;
      return;
    }

    for (int n_char = 0; n_char < packet->to_string().size(); ++n_char) {
      if (packet->to_string().at(n_char) == "-1")
        app->w_courtroom->set_taken(n_char, true);
      else
        app->w_courtroom->set_taken(n_char, false);
    }
    logToDemo = false;
}
void CT::handler(AOApplication* app, AOPacket* packet) {
      if (!app->courtroom_constructed || packet->get_contents().size() < 2) {
        return;
       }

      if (packet->get_contents().size() == 3)
        app->w_courtroom->append_server_chatmessage(
            packet->get_contents().at(0), packet->get_contents().at(1), packet->get_contents().at(2));
      else
        app->w_courtroom->append_server_chatmessage(packet->get_contents().at(0),
                                               packet->get_contents().at(1), "0");
}
void FL::handler(AOApplication* app, AOPacket* packet){
      QString f_packet = packet->to_string();
      app->yellow_text_supported = false;
      app->prezoom_supported = false;
      app->flipping_supported = false;
      app->custom_objection_supported = false;
      app->desk_mod_supported = false;
      app->evidence_supported = false;
      app->cccc_ic_supported = false;
      app->arup_supported = false;
      app->casing_alerts_supported = false;
      app->modcall_reason_supported = false;
      app->looping_sfx_supported = false;
      app->additive_text_supported = false;
      app->effects_supported = false;
      app->expanded_desk_mods_supported = false;
      app->auth_packet_supported = false;
      if (f_packet.contains("yellowtext", Qt::CaseInsensitive))
        app->yellow_text_supported = true;
      if (f_packet.contains("prezoom", Qt::CaseInsensitive))
        app->prezoom_supported = true;
      if (f_packet.contains("flipping", Qt::CaseInsensitive))
        app->flipping_supported = true;
      if (f_packet.contains("customobjections", Qt::CaseInsensitive))
        app->custom_objection_supported = true;
      if (f_packet.contains("deskmod", Qt::CaseInsensitive))
        app->desk_mod_supported = true;
      if (f_packet.contains("evidence", Qt::CaseInsensitive))
        app->evidence_supported = true;
      if (f_packet.contains("cccc_ic_support", Qt::CaseInsensitive))
        app->cccc_ic_supported = true;
      if (f_packet.contains("arup", Qt::CaseInsensitive))
        app->arup_supported = true;
      if (f_packet.contains("casing_alerts", Qt::CaseInsensitive))
        app->casing_alerts_supported = true;
      if (f_packet.contains("modcall_reason", Qt::CaseInsensitive))
        app->modcall_reason_supported = true;
      if (f_packet.contains("looping_sfx", Qt::CaseInsensitive))
        app->looping_sfx_supported = true;
      if (f_packet.contains("additive", Qt::CaseInsensitive))
        app->additive_text_supported = true;
      if (f_packet.contains("effects", Qt::CaseInsensitive))
        app->effects_supported = true;
      if (f_packet.contains("y_offset", Qt::CaseInsensitive))
          app->y_offset_supported = true;
      if (f_packet.contains("expanded_desk_mods", Qt::CaseInsensitive))
        app->expanded_desk_mods_supported = true;
      if (f_packet.contains("auth_packet", Qt::CaseInsensitive))
        app->auth_packet_supported = true;
      logToDemo = false;

}
void PN::handler(AOApplication* app, AOPacket* packet){
    QStringList f_contents = packet->get_contents();
    if (!app->lobby_constructed || f_contents.size() < 2)
        return;

      app->w_lobby->set_player_count(f_contents.at(0).toInt(),
                                f_contents.at(1).toInt());

      if (f_contents.size() >= 3) {
          app->w_lobby->set_server_description(f_contents.at(2));
      }

      if (app->w_lobby->doubleclicked) {
        app->send_server_packet(new AOPacket("askchaa"));
        app->w_lobby->doubleclicked = false;
      }
      logToDemo = false;

}
