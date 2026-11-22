#pragma once

#include "lib/const.h"

namespace messenger {
// Main messages in the queue
struct UserTextMsg {
    // Since queue size is strictly limited by semaphore initial size, there is no point in dealing
    // with variable-length messages
    char payload[kMaxPayloadSize];
};
}  // namespace messenger
