#pragma once

#include <cstddef>

namespace messenger::system {
/// @brief An owning handle to a shared memory region. Owns the kernel structure
class SharedMemoryOwner {
public:
    explicit SharedMemoryOwner(std::size_t byte_size);
    ~SharedMemoryOwner();

    std::size_t Size() const noexcept {
        return size_;
    }

    int Id() const noexcept {
        return id_;
    }

    void* Data() const noexcept {
        return data_;
    }

private:
    std::size_t size_ = 0;
    int id_ = -1;
    void* data_ = nullptr;
};

/// @brief A handle to a shared memory region. Only owns the memory map
class SharedMemoryMap {
public:
    explicit SharedMemoryMap(int id);
    ~SharedMemoryMap();

    std::size_t Size() const noexcept {
        return size_;
    }

    int Id() const noexcept {
        return id_;
    }

    void* Data() const noexcept {
        return data_;
    }

private:
    std::size_t size_ = 0;
    int id_ = -1;
    void* data_ = nullptr;
};
}  // namespace messenger::system
