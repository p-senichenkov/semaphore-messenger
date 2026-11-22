#include "lib/core/system/shared_memory.h"

#include <cstddef>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "lib/core/system/system_error.h"

using namespace messenger::system;

namespace {
void* Attach(int id) {
    void* p = shmat(id, nullptr, 0);
    if (p == reinterpret_cast<void*>(-1)) {
        throw SystemError{"shmat"};
    }
    return p;
}

void Detach(void const* data) {
    if (data) {
        shmdt(data);
    }
}
}  // namespace

SharedMemoryOwner::SharedMemoryOwner(std::size_t byte_size) : size_(byte_size) {
    if (byte_size == 0) {
        throw std::invalid_argument("zero-sized shared memory");
    }

    id_ = shmget(IPC_PRIVATE, byte_size, IPC_CREAT | 0600);
    if (id_ == -1) {
        throw SystemError{"shmget"};
    }

    try {
        data_ = Attach(id_);
    } catch (std::exception const&) {
        shmctl(id_, IPC_RMID, nullptr);
        throw;
    }
}

SharedMemoryOwner::~SharedMemoryOwner() {
    Detach(data_);

    if (id_ >= 0) {
        shmctl(id_, IPC_RMID, nullptr);
    }
}

SharedMemoryMap::SharedMemoryMap(int id) {
    shmid_ds info{};
    if (shmctl(id, IPC_STAT, &info) == -1) {
        throw SystemError{"shmctl(IPC_STAT)"};
    }

    size_ = info.shm_segsz;
    id_ = id;
    data_ = Attach(id_);
}

SharedMemoryMap::~SharedMemoryMap() {
    Detach(data_);
}
