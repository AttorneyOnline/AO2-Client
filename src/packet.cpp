#include "packet.h"
#include "aoapplication.h"
#include "aopacket.h"
#include "courtroom.h"

void MS::handler(AOApplication* app, AOPacket* packet) {
    if (app->courtroom_constructed && app->courtroom_loaded)
    {
      app->w_courtroom->chatmessage_enqueue(packet->get_contents());
    }
}

void CharsCheck::handler(AOApplication* app, AOPacket* packet) {
    if (!app->courtroom_constructed)
      goto end;

    for (int n_char = 0; n_char < packet->to_string().size(); ++n_char) {
      if (packet->to_string().at(n_char) == "-1")
        app->w_courtroom->set_taken(n_char, true);
      else
        app->w_courtroom->set_taken(n_char, false);
    }
end:
    delete packet;
}
