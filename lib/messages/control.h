#pragma once

#include <cstddef>
#include <cstdint>

namespace messenger {
enum class EControl {
    // Send IDs of IPC objects
    kIpcIDs,
    // Accept connection and start messaging
    kAck,
};

struct ControlMsg {
    EControl kind;
    uint16_t size;
    // TODO: maybe union?))
    std::byte const* payload;
};

struct IpcIDs {
    // TODO:
};
}  // namespace messenger
