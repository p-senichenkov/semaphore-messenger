#pragma once

namespace messenger {
constexpr static auto kMaxPayloadSizeBytes = 255;

struct UserTextMsg {
    // Since queue size is strictly limited by semaphore initial size, there is no point in dealing
    // with variable-length messages
    char payload[kMaxPayloadSizeBytes];
};
}  // namespace messenger
