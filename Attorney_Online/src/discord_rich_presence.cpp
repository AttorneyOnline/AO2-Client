#include "discord_rich_presence.h"

namespace AttorneyOnline {

AORPC::AORPC()
{
    discord::Result result = discord::Core::Create(APPLICATION_ID, DiscordCreateFlags_NoRequireDiscord, &core);
    core->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, std::string message) {
        qDebug() << "Discord:" << &level << " - " << &message;
    });
    Q_UNUSED(result)
}

AttorneyOnline::AORPC::~AORPC()
{

}

void AttorneyOnline::AORPC::state_lobby()
{

}

void AttorneyOnline::AORPC::state_server(std::string name, std::string server_id)
{
    Q_UNUSED(name)
    Q_UNUSED(server_id)
}

void AttorneyOnline::AORPC::state_character(std::string name)
{
    Q_UNUSED(name)
}

void AttorneyOnline::AORPC::state_spectate()
{

}

} // namespace AttorneyOnline
