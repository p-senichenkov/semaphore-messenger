#pragma once

#include <initializer_list>
#include <utility>
#include <vector>

#include "lib/system/semaphore.h"

namespace messenger::system {
/// @brief An owning handle to a semaphore set
class SemaphoreSet {
public:
    explicit SemaphoreSet(std::initializer_list<unsigned short> initial_values);

    SemaphoreSet(SemaphoreSet const&) = delete;
    SemaphoreSet& operator=(SemaphoreSet const&) = delete;

    SemaphoreSet(SemaphoreSet&& other)
        : count_(std::exchange(other.count_, 0)), id_(std::exchange(other.id_, -1)) {}

    SemaphoreSet& operator=(SemaphoreSet&& other) {
        SemaphoreSet tmp{std::move(other)};
        Swap(tmp);
        return *this;
    }

    ~SemaphoreSet();

    int Id() const noexcept {
        return id_;
    }

    std::vector<Semaphore> Semaphores() const;

private:
    void Swap(SemaphoreSet& other) {
        std::swap(count_, other.count_);
        std::swap(id_, other.id_);
    }

    unsigned short count_;
    int id_;
};

/// @brief Non-owning view of a semaphore set
class SemaphoreSetView {
public:
    explicit SemaphoreSetView(int id);

    SemaphoreSetView(SemaphoreSetView const&) = delete;
    SemaphoreSetView& operator=(SemaphoreSetView const&) = delete;
    SemaphoreSetView(SemaphoreSetView&&) = default;
    SemaphoreSetView& operator=(SemaphoreSetView&&) = default;

    int Id() const noexcept {
        return id_;
    }

    std::vector<Semaphore> Semaphores() const;

private:
    unsigned short count_;
    int id_;
};
}  // namespace messenger::system
