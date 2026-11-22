#pragma once

#include "lib/core/resources/holder.h"

namespace messenger::handshake {
// Use free functions to ensure that socket is always closed as soon as it's not needed anymore
IpcResourcesOwner DoMasterHandshake(int pid);
IpcResourcesView DoSlaveHandshake(int master_pid);
}  // namespace messenger::handshake
