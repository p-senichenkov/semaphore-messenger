#pragma once

#include <cstddef>
#include <utility>

namespace messenger::system {
/// @brief An owning handle to a shared memory region. Owns the kernel structure
class SharedMemoryOwner {
public:
    explicit SharedMemoryOwner(std::size_t byte_size);

    SharedMemoryOwner(SharedMemoryOwner const&) = delete;
    SharedMemoryOwner& operator=(SharedMemoryOwner const&) = delete;

    SharedMemoryOwner(SharedMemoryOwner&& other)
        : size_(std::exchange(other.size_, 0)),
          id_(std::exchange(other.id_, -1)),
          data_(std::exchange(other.data_, nullptr)) {}

    SharedMemoryOwner& operator=(SharedMemoryOwner&& other) {
        SharedMemoryOwner tmp{std::move(other)};
        Swap(tmp);
        return *this;
    }

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
    void Swap(SharedMemoryOwner& other) {
        std::swap(size_, other.size_);
        std::swap(id_, other.id_);
        std::swap(data_, other.data_);
    }

    std::size_t size_ = 0;
    int id_ = -1;
    void* data_ = nullptr;
};

/// @brief A handle to a shared memory region. Only owns the memory map
class SharedMemoryMap {
public:
    explicit SharedMemoryMap(int id);

    SharedMemoryMap(SharedMemoryMap const&) = delete;
    SharedMemoryMap& operator=(SharedMemoryMap const&) = delete;

    SharedMemoryMap(SharedMemoryMap&& other)
        : size_(std::exchange(other.size_, 0)),
          id_(std::exchange(other.id_, -1)),
          data_(std::exchange(other.data_, nullptr)) {}

    SharedMemoryMap& operator=(SharedMemoryMap&& other) {
        SharedMemoryMap tmp{std::move(other)};
        Swap(tmp);
        return *this;
    }

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
    void Swap(SharedMemoryMap& other) {
        std::swap(size_, other.size_);
        std::swap(id_, other.id_);
        std::swap(data_, other.data_);
    }

    std::size_t size_ = 0;
    int id_ = -1;
    void* data_ = nullptr;
};
}  // namespace messenger::system
