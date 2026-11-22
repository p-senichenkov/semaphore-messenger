#include "lib/core/handshake/handshake.h"

#include <exception>
#include <utility>

#include "lib/handshake/handshake_connection.h"
#include "lib/handshake/socket_utils.h"
#include "lib/messages/handshake.h"
#include "lib/resources/holder.h"
#include "lib/resources/memory_layout.h"

using namespace messenger;
using namespace messenger::handshake;

namespace {
std::pair<IpcResourcesOwner, IpcIDs> MakeIpcResources() {
    constexpr static auto kSharedMemoryByteSize = sizeof(MemoryLayout);
    constexpr static IPCResourceIndices kFixedIpcResourceIndices{
            .master_queue_idx = 0,
            .slave_queue_idx = 1,

            .master_read_sem_idx = 0,
            .master_write_sem_idx = 1,
            .slave_read_sem_idx = 2,
            .slave_write_sem_idx = 3,
    };
    constexpr static std::initializer_list<unsigned short> kSemaphoreInitialValues{
            /* Master read = */ 0,
            /* Master write = */ 1,
            /* Slave read = */ 0,
            /* Slave write = */ 1,
    };

    IpcResourcesOwner owner{kSharedMemoryByteSize, kSemaphoreInitialValues,
                            kFixedIpcResourceIndices};

    IpcIDs ids{
            .shared_memory_id = owner.SharedMemoryId(),
            .semaphore_set_id = owner.SemaphoreSetId(),
            .indices = kFixedIpcResourceIndices,
    };
    return {owner, ids};
}
}  // namespace

IpcResourcesOwner DoMasterHandshake(int pid) {
    SocketMaster socket_master{pid};

    // TODO: Timeout here?
    auto conn = socket_master.AwaitConnection();
    HandshakeConnection connection{std::move(conn)};

    try {
        connection.Recieve(EControlMsgKind::kReq);

        auto [resources, ids] = MakeIpcResources();
        connection.SendIpcIDs(ids);

        connection.Recieve(EControlMsgKind::kAck);

        return resources;
    } catch (std::exception const&) {
        connection.SendResetConnection();
        throw;
    }
}

IpcResourcesView DoSlaveHandshake(int master_pid) {
    HandshakeConnection connection{SocketSlave(master_pid)};

    try {
        connection.SendRequestConnection();

        auto ipc_ids_msg = connection.Recieve(EControlMsgKind::kIpcIDs);
        auto ids = ipc_ids_msg.payload.ipc_ids;
        IpcResourcesView view{ids.shared_memory_id, ids.semaphore_set_id, ids.indices};

        connection.SendAcceptConnection();

        return view;
    } catch (std::exception const&) {
        connection.SendResetConnection();
        throw;
    }
}
