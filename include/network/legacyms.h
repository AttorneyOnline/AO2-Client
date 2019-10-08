#ifndef LEGACYMS_H
#define LEGACYMS_H

#include "masterserver.h"

namespace AttorneyOnline {

/*!
 * \brief An AO2-compatible master server client.
 *
 * Its functionality is very simple; however, it only allows for manual
 * polling of servers. The server makes no attempt to broadcast server list
 * changes to clients. Like the \link{LegacyClient}, this implementation
 * makes no attempt to be AO1-compatible.
 */
class LegacyMasterServer : public MasterServer
{

};

}


#endif // LEGACYMS_H
