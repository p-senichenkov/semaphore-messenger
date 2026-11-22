#pragma once

#include <string>

#include "lib/core/messages/handshake.h"
#include "lib/core/system/abstract_socket.h"

namespace messenger::handshake {
class Connection {
public:
    explicit Connection(system::AbstractSocket socket) : socket_(socket) {}

    void Send(ControlMsg const& msg) {
        socket_.Send(msg);
    }

    ControlMsg Receive() {
        return socket_.Receive<ControlMsg>();
    }

private:
    system::AbstractSocket socket_;
};

class SocketMaster {
public:
    explicit SocketMaster(int pid);

    Connection AwaitConnection() {
        return Connection{socket_.Accept(name_)};
    }

private:
    std::string name_;
    system::AbstractSocket socket_;
};

Connection SocketSlave(int pid);
}  // namespace messenger::handshake
