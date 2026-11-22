#include "lib/core/handshake/socket_utils.h"

#include <string>

#include "lib/core/system/abstract_socket.h"

using namespace messenger;
using namespace messenger::handshake;

namespace {
std::string MakeName(int pid) {
    return "semaphore-messenger-" + std::to_string(pid);
}
}  // namespace

SocketMaster::SocketMaster(int pid) : name_(MakeName(pid)), socket_(system::Listen(name_)) {}

Connection SocketSlave(int pid) {
    return Connection{system::Connect(MakeName(pid))};
}
