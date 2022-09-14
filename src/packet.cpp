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
