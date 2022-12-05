#include "discord_rich_presence.h"

namespace AttorneyOnline {

AORPC::AORPC()
{
    // Pretty sure this is correct.
    discord::Core* core_raw{};
    auto result = discord::Core::Create(client_id, DiscordCreateFlags_NoRequireDiscord, &core_raw);
    core = std::unique_ptr<discord::Core>(core_raw);
    if(result != discord::Result::Ok) {
        qDebug() << "DISCORD: Unable to create Discord RPC. Error code: " << static_cast<int>(result);
    }
    else {
        qDebug() << "DISCORD: RPC ready!";
    }

    activity.SetName("Attorney Online 2");
    activity.SetType(discord::ActivityType::Playing);
    activity.SetInstance(true);
    activity.GetAssets().SetLargeImage("ao2-logo");
    activity.GetAssets().SetLargeText("Objection!");

    connect(&rpc_beat, &QTimer::timeout, this, &AORPC::publish_rpc);
    rpc_beat.start(RPC_HEARTBEAT);
}

void AORPC::publish_rpc()
{
    // I am not even sure if this is the right way to do it.
    core->ActivityManager().UpdateActivity(activity, nullptr);
    core->RunCallbacks();
    rpc_beat.start(RPC_HEARTBEAT);
}

void AORPC::state_lobby()
{
    qDebug() << "DISCORD: Setting Lobby state.";
    activity.SetState("In Lobby");
    activity.SetDetails("Idle");
    publish_rpc();
}

void AORPC::state_server(std::string name, std::string server_id)
{
    //TODO : This code is terrible and likely will not display the timestamp correctly.
    auto timestamp = static_cast<int64_t>(std::time(nullptr));
    activity.SetState("In a Server");
    activity.SetDetails(name.c_str());
    activity.GetTimestamps().SetStart(this->timestamp);
    activity.GetSecrets().SetMatch(server_id.c_str());
    this->server_id = server_id;
    this->server_name = name;
    this->timestamp = timestamp;
}

void AORPC::state_character(std::string name)
{
    Q_UNUSED(name)
}

void AORPC::state_spectate()
{

}

} // namespace AttorneyOnline
