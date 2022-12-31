#include "courtroom2.h"

Courtroom2::Courtroom2(std::shared_ptr<Client> client) :
    QMainWindow(), client(client)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

Courtroom2::~Courtroom2()
{

}
