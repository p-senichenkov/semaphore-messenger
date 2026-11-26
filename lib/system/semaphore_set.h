#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

#include <sys/sem.h>

#include "lib/system/semaphore.h"

namespace messenger::system {
/// @brief An owning handle to a semaphore set
class SemaphoreSet {
public:
    explicit SemaphoreSet(std::initializer_list<unsigned short> initial_values);

    ~SemaphoreSet() {
        Reset();
    }

    int Id() const noexcept {
        return id_;
    }

    std::vector<Semaphore> Semaphores() const;

private:
    unsigned short count_;
    int id_;

    void Reset() noexcept;
};

/// @brief Non-owning view of a semaphore set
class SemaphoreSetView {
public:
    explicit SemaphoreSetView(int id);

    int Id() const noexcept {
        return id_;
    }

    std::vector<Semaphore> Semaphores() const;

private:
    unsigned short count_;
    int id_;
};
}  // namespace messenger::system
