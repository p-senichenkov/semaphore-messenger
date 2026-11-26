#pragma once

#include <stdexcept>
#include <utility>

#include "lib/handshake/socket_utils.h"
#include "lib/messages/handshake.h"

namespace messenger::handshake {
class HandshakeConnection {
public:
    struct ConnectionReset : public std::runtime_error {
        ConnectionReset() : std::runtime_error("Connection reset by peer") {}
    };

    struct InvalidControlMessageKind : public std::runtime_error {
        InvalidControlMessageKind(EControlMsgKind expected, EControlMsgKind actual)
            : std::runtime_error("Got unexpected Control message kind: expected " +
                                 ControlMsgKindToString(expected) + ", got " +
                                 ControlMsgKindToString(actual)) {}
    };

    HandshakeConnection(Connection&& connection) : connection_(std::move(connection)) {}

    void SendRequestConnection() {
        ControlMsg msg = KindOnly(EControlMsgKind::kReq);
        connection_.Send(msg);
    }

    void SendIpcIDs(IpcIDs const& ids) {
        ControlMsg msg{.kind = EControlMsgKind::kIpcIDs, .payload = {.ipc_ids = ids}};
        connection_.Send(msg);
    }

    void SendAcceptConnection() {
        ControlMsg msg = KindOnly(EControlMsgKind::kAck);
        connection_.Send(msg);
    }

    void SendResetConnection() {
        ControlMsg msg = KindOnly(EControlMsgKind::kRst);
        connection_.Send(msg);
    }

    ControlMsg Recieve(EControlMsgKind expected_kind) {
        ControlMsg msg = connection_.Receive();
        if (msg.kind == expected_kind) {
            return msg;
        }
        if (msg.kind == EControlMsgKind::kRst) {
            throw ConnectionReset{};
        }
        throw InvalidControlMessageKind{expected_kind, msg.kind};
    }

private:
    Connection connection_;
};
}  // namespace messenger::handshake
