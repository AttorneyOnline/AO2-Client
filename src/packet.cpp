#include "packet.h"
#include "aoapplication.h"
#include "aopacket.h"
#include "courtroom.h"
#include "hardware_functions.h"

void Decryptor::handler(AOApplication* app, AOPacket* packet){
    if (packet->to_string().size() == 0) {
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
void SI::handler(AOApplication* app, AOPacket* packet){
    QStringList f_contents = packet->get_contents();
    if (!app->lobby_constructed || f_contents.size() != 3) {
        return;
    }

    app->char_list_size = f_contents.at(0).toInt();
    app->evidence_list_size = f_contents.at(1).toInt();
    app->music_list_size = f_contents.at(2).toInt();

    if (app->char_list_size < 0 || app->evidence_list_size < 0 || app->music_list_size < 0)
        return;

    app->loaded_chars = 0;
    app->loaded_evidence = 0;
    app->loaded_music = 0;
    app->generated_chars = 0;

    app->destruct_courtroom();
    app->construct_courtroom();

    app->courtroom_loaded = false;

    int selected_server = app->w_lobby->get_selected_server();

    QString server_address = "", server_name = "";
    if (app->w_lobby->public_servers_selected) {
        if (selected_server >= 0 && selected_server < app->get_server_list().size()) {
            auto info = app->get_server_list().at(selected_server);
            server_name = info.name;
            server_address =
                    QString("%1:%2").arg(info.ip, QString::number(info.port));
            app->window_title = server_name;
        }
    }
    else {
        if (selected_server >= 0 && selected_server < app->get_favorite_list().size()) {
            auto info = app->get_favorite_list().at(selected_server);
            server_name = info.name;
            server_address =
                    QString("%1:%2").arg(info.ip, QString::number(info.port));
            app->window_title = server_name;
        }
    }

    if (app->courtroom_constructed)
        app->w_courtroom->set_window_title(app->window_title);

    app->w_lobby->show_loading_overlay();
    app->w_lobby->set_loading_text(app->tr("Loading"));
    app->w_lobby->set_loading_value(0);

    AOPacket *f_packet;

    f_packet = new AOPacket("RC");
    app->send_server_packet(f_packet);

    // Remove any characters not accepted in folder names for the server_name
    // here
    if (app->get_demo_logging_enabled() && server_name != "Demo playback") {
        app->log_filename = QDateTime::currentDateTime().toUTC().toString(
                    "'logs/" + server_name.remove(QRegExp("[\\\\/:*?\"<>|\']")) +
                    "/'yyyy-MM-dd hh-mm-ss t'.log'");
        app->write_to_file("Joined server " + server_name + " hosted on address " +
                           server_address + " on " +
                           QDateTime::currentDateTime().toUTC().toString(),
                           app->log_filename, true);
    }
    else
        app->log_filename = "";

    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(server_address.toUtf8());
    if (app->is_discord_enabled())
        app->discord->state_server(server_name.toStdString(),
                                   hash.result().toBase64().toStdString());
    logToDemo = false;
}

void SC::handler(AOApplication* app, AOPacket* packet) {
    QStringList f_contents = packet->get_contents();
    if (!app->courtroom_constructed)
        return;

    app->w_courtroom->clear_chars();
    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
        QStringList sub_elements = f_contents.at(n_element).split("&");

        AOPacket::unescape(sub_elements);

        char_type f_char;
        f_char.name = sub_elements.at(0);
        if (sub_elements.size() >= 2)
            f_char.description = sub_elements.at(1);

        // temporary. the CharsCheck packet sets this properly
        f_char.taken = false;

        app->w_courtroom->append_char(f_char);

        if (!app->courtroom_loaded) {
            ++app->loaded_chars;
            app->w_lobby->set_loading_text(app->tr("Loading chars:\n%1/%2")
                                           .arg(QString::number(app->loaded_chars))
                                           .arg(QString::number(app->char_list_size)));

            int total_loading_size =
                    app->char_list_size * 2 + app->evidence_list_size + app->music_list_size;
            int loading_value = int(
                        ((app->loaded_chars + app->generated_chars + app->loaded_music + app->loaded_evidence) /
                         static_cast<double>(total_loading_size)) *
                        100);
            app->w_lobby->set_loading_value(loading_value);
        }
    }

    if (!app->courtroom_loaded)
        app->send_server_packet(new AOPacket("RM"));
    else
        app->w_courtroom->character_loading_finished();
}

void SM::handler(AOApplication* app, AOPacket* packet) {
    QStringList f_contents = packet->get_contents();
    if (!app->courtroom_constructed || app->courtroom_loaded)
        return;

    bool musics_time = false;
    int areas = 0;

    for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
        ++app->loaded_music;

        app->w_lobby->set_loading_text(app->tr("Loading music:\n%1/%2")
                                       .arg(QString::number(app->loaded_music))
                                       .arg(QString::number(app->music_list_size)));

        if (musics_time) {
            app->w_courtroom->append_music(f_contents.at(n_element));
        }
        else {
            if (f_contents.at(n_element).endsWith(".wav") ||
                    f_contents.at(n_element).endsWith(".mp3") ||
                    f_contents.at(n_element).endsWith(".mp4") ||
                    f_contents.at(n_element).endsWith(".ogg") ||
                    f_contents.at(n_element).endsWith(".opus")) {
                musics_time = true;
                app->w_courtroom->fix_last_area();
                app->w_courtroom->append_music(f_contents.at(n_element));
                areas--;
            }
            else {
                app->w_courtroom->append_area(f_contents.at(n_element));
                areas++;
            }
        }

        int total_loading_size =
                app->char_list_size * 2 + app->evidence_list_size + app->music_list_size;
        int loading_value = int(
                    ((app->loaded_chars + app->generated_chars + app->loaded_music + app->loaded_evidence) /
                     static_cast<double>(total_loading_size)) *
                    100);
        app->w_lobby->set_loading_value(loading_value);
    }
    for (int area_n = 0; area_n < areas; area_n++) {
        app->w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
    }

    app->send_server_packet(new AOPacket("RD"));
    logToDemo = false;
}

void FM::handler(AOApplication* app, AOPacket* packet) {
        QStringList f_contents = packet->get_contents();
        if (!app->courtroom_constructed)
          return;

        app->w_courtroom->clear_music();

        for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
          app->w_courtroom->append_music(f_contents.at(n_element));
        }

        app->w_courtroom->list_music();
        logToDemo = false;


}
void FA::handler(AOApplication* app, AOPacket* packet) // Fetch areas ONLY
{
    QStringList f_contents = packet->get_contents();
  if (!app->courtroom_constructed)
    return;

  app->w_courtroom->clear_areas();
  app->w_courtroom->arup_clear();

  for (int n_element = 0; n_element < f_contents.size(); ++n_element) {
    app->w_courtroom->append_area(f_contents.at(n_element));
    app->w_courtroom->arup_append(0, "Unknown", "Unknown", "Unknown");
  }

  app->w_courtroom->list_areas();
  logToDemo = false;
}

void DONE::handler(AOApplication* app, AOPacket* packet) {

        if (!app->courtroom_constructed)
          return;

        if (app->lobby_constructed)
          app->w_courtroom->append_server_chatmessage(app->tr("[Global log]"),
                                                 app->w_lobby->get_chatlog(), "0");

        app->w_courtroom->character_loading_finished();
        app->w_courtroom->done_received();

        app->courtroom_loaded = true;

        app->destruct_lobby();
        logToDemo = false;


}


