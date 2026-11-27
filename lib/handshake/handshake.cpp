#include "lib/handshake/handshake.h"

#include <exception>
#include <memory>
#include <unistd.h>
#include <utility>

#include "lib/handshake/handshake_connection.h"
#include "lib/handshake/socket_utils.h"
#include "lib/messages/handshake.h"
#include "lib/resources/holder.h"
#include "lib/resources/memory_layout.h"
#include "lib/util/log.h"

namespace messenger::handshake {
namespace {
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
}  // namespace

std::unique_ptr<IpcResourcesOwner> DoMasterHandshake(pid_t pid) {
    LOG(Debug) << "Master";

    SocketMaster socket_master{pid};

    // TODO: Timeout here?
    auto conn = socket_master.AwaitConnection();
    HandshakeConnection connection{std::move(conn)};

    try {
        connection.Recieve(EControlMsgKind::kReq);

        auto resources = std::make_unique<IpcResourcesOwner>(
                kSharedMemoryByteSize, kSemaphoreInitialValues, kFixedIpcResourceIndices);
        IpcIDs ids{

                .shared_memory_id = resources->SharedMemoryId(),
                .semaphore_set_id = resources->SemaphoreSetId(),
                .indices = kFixedIpcResourceIndices,
        };
        connection.SendIpcIDs(std::move(ids));

        connection.Recieve(EControlMsgKind::kAck);

        return resources;
    } catch (std::exception const&) {
        connection.SendResetConnection();
        throw;
    }
}

std::unique_ptr<IpcResourcesView> DoSlaveHandshake(pid_t master_pid) {
    LOG(Debug) << "Slave";

    HandshakeConnection connection{SocketSlave(master_pid)};

    try {
        connection.SendRequestConnection();

        auto ipc_ids_msg = connection.Recieve(EControlMsgKind::kIpcIDs);
        auto ids = ipc_ids_msg.payload.ipc_ids;
        auto view = std::make_unique<IpcResourcesView>(ids.shared_memory_id, ids.semaphore_set_id,
                                                       ids.indices);

        connection.SendAcceptConnection();

        return view;
    } catch (std::exception const&) {
        connection.SendResetConnection();
        throw;
    }
}
}  // namespace messenger::handshake
