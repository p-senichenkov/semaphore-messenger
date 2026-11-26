#pragma once

#include <memory>
#include <unistd.h>

#include "lib/resources/holder.h"

namespace messenger::handshake {
// Use free functions to ensure that socket is always closed as soon as it's not needed anymore
std::unique_ptr<IpcResourcesOwner> DoMasterHandshake(pid_t pid);
std::unique_ptr<IpcResourcesView> DoSlaveHandshake(pid_t master_pid);
}  // namespace messenger::handshake
