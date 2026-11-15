#pragma once

#include "lib/messages/control_area.h"
#include "lib/messages/user_text.h"

namespace messenger {
/// @brief Circular buffer for user text messages
struct MessageQueue {
    UserTextMsg messages[kQueueSize];
};

// The exact shared memory layout
struct MemoryLayout {
    // A single byte for "highest-priority" control signals, that should bypass synchronization
    EControlArea control_area;
    // Two uni-directional message queues
    MessageQueue queues[2];
};

}  // namespace messenger
