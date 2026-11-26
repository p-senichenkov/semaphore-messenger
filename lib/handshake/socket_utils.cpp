#include "lib/handshake/socket_utils.h"

#include <string>

#include "lib/system/abstract_socket.h"

namespace {
std::string MakeName(int pid) {
    return "semaphore-messenger-" + std::to_string(pid);
}
}  // namespace

namespace messenger::handshake {
SocketMaster::SocketMaster(int pid) : name_(MakeName(pid)), socket_(system::Listen(name_)) {}

Connection SocketSlave(int pid) {
    return Connection{system::Connect(MakeName(pid))};
}
}  // namespace messenger::handshake
