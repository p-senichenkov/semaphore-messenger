#pragma once

#include <cstdint>

namespace messenger {
// I don't want to deal with synchronization when delivering "highest-priority" control messages,
// such as shutdown signal. So I keep a single byte at the beginning of shared memory for such
// control signals
enum class EControlArea : uint8_t {
    // Shutdown immediately (and clean-up all IPC resources)
    kShutdown = 1,
    // Other values are reserved for future use
};
}  // namespace messenger
