#include "network/client.h"

namespace AttorneyOnline {

// Must be defined in order to emit a vtable - otherwise, we end up with
// a nasty compiler warning.
Client::~Client() {}

} // namespace AttorneyOnline
