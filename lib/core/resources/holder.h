#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

#include "lib/core/messages/control_area.h"
#include "lib/core/messages/handshake.h"
#include "lib/core/messages/user_text.h"
#include "lib/core/resources/memory_layout.h"
#include "lib/core/system/semaphore.h"
#include "lib/core/system/semaphore_set.h"
#include "lib/core/system/shared_memory.h"

namespace messenger {
class IIpcResources {
public:
    virtual EControlArea& ControlArea() noexcept = 0;

    virtual UserTextMsg& OurMsgPlace() noexcept = 0;
    virtual UserTextMsg& TheirMsgPlace() noexcept = 0;

    virtual system::Semaphore OurReadSemaphore() noexcept = 0;
    virtual system::Semaphore OurWriteSemaphore() noexcept = 0;
    virtual system::Semaphore TheirReadSemaphore() noexcept = 0;
    virtual system::Semaphore TheirWriteSemaphore() noexcept = 0;

    // Handshake socket should be closed after the handshake, so we don't keep it here
};

class IpcResourcesBase : public IIpcResources {
public:
    IpcResourcesBase(unsigned char our_queue_idx, unsigned char their_queue_idx,
                     unsigned short our_read_sem_idx, unsigned short our_write_sem_idx,
                     unsigned short their_read_sem_idx, unsigned short their_write_sem_idx)
        : our_queue_idx_(our_queue_idx),
          their_queue_idx_(their_queue_idx),
          our_read_sem_idx_(our_read_sem_idx),
          their_write_sem_idx_(their_write_sem_idx) {}

    EControlArea& ControlArea() noexcept final {
        return GetMemoryLayout().control_area;
    }

    UserTextMsg& OurMsgPlace() noexcept final {
        return GetMemoryLayout().msg_places[our_queue_idx_];
    }

    UserTextMsg& TheirMsgPlace() noexcept final {
        return GetMemoryLayout().msg_places[their_queue_idx_];
    }

    system::Semaphore OurReadSemaphore() noexcept final {
        return Semaphores()[our_read_sem_idx_];
    }

    system::Semaphore OurWriteSemaphore() noexcept final {
        return Semaphores()[our_write_sem_idx_];
    }

    system::Semaphore TheirReadSemaphore() noexcept final {
        return Semaphores()[their_read_sem_idx_];
    }

    system::Semaphore TheirWriteSemaphore() noexcept final {
        return Semaphores()[their_write_sem_idx_];
    }

protected:
    virtual MemoryLayout& GetMemoryLayout() noexcept = 0;
    virtual std::vector<system::Semaphore>& Semaphores() noexcept = 0;

private:
    unsigned char our_queue_idx_;
    unsigned char their_queue_idx_;

    unsigned short our_read_sem_idx_;
    unsigned short our_write_sem_idx_;
    unsigned short their_read_sem_idx_;
    unsigned short their_write_sem_idx_;
};

class IpcResourcesOwner final : public IpcResourcesBase {
public:
    IpcResourcesOwner(std::size_t shm_byte_size,
                      std::initializer_list<unsigned short> sem_initial_values,
                      IPCResourceIndices const& indices)
        : IpcResourcesBase(indices.master_queue_idx, indices.slave_queue_idx,
                           indices.master_read_sem_idx, indices.master_write_sem_idx,
                           indices.slave_read_sem_idx, indices.slave_write_sem_idx),
          shared_memory_(shm_byte_size),
          memory_layout_(reinterpret_cast<MemoryLayout*>(shared_memory_.Data())),
          semaphore_set_(std::move(sem_initial_values)),
          semaphores_(semaphore_set_.Semaphores()) {}

    MemoryLayout& GetMemoryLayout() noexcept override {
        return *memory_layout_;
    }

    std::vector<system::Semaphore>& Semaphores() noexcept override {
        return semaphores_;
    }

    int SharedMemoryId() const noexcept {
        return shared_memory_.Id();
    }

    int SemaphoreSetId() const noexcept {
        return semaphore_set_.Id();
    }

private:
    system::SharedMemoryOwner shared_memory_;
    MemoryLayout* memory_layout_;

    system::SemaphoreSet semaphore_set_;
    std::vector<system::Semaphore> semaphores_;
};

class IpcResourcesView final : public IpcResourcesBase {
public:
    IpcResourcesView(int shm_id, int sem_set_id, IPCResourceIndices const& indices)
        : IpcResourcesBase(indices.slave_queue_idx, indices.master_queue_idx,
                           indices.slave_read_sem_idx, indices.slave_read_sem_idx,
                           indices.master_read_sem_idx, indices.master_write_sem_idx),
          shared_memory_(shm_id),
          memory_layout_(reinterpret_cast<MemoryLayout*>(shared_memory_.Data())),
          semaphore_set_(sem_set_id),
          semaphores_(semaphore_set_.Semaphores()) {}

    MemoryLayout& GetMemoryLayout() noexcept override {
        return *memory_layout_;
    }

    std::vector<system::Semaphore>& Semaphores() noexcept override {
        return semaphores_;
    }

private:
    system::SharedMemoryMap shared_memory_;
    MemoryLayout* memory_layout_;

    system::SemaphoreSetView semaphore_set_;
    std::vector<system::Semaphore> semaphores_;
};
}  // namespace messenger
