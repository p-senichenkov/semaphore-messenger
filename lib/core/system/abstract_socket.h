#pragma once

#include <cstddef>
#include <string_view>

namespace messenger::system {

/// @brief An owning handle to an abstract UNIX socket
class AbstractSocket {
public:
    explicit AbstractSocket(int fd) noexcept : fd_(fd) {}

    ~AbstractSocket();

    int Fd() const noexcept {
        return fd_;
    }

    AbstractSocket Accept(std::string_view name) const;

    template <typename T>
    void Send(T const& value) const {
        SendRaw(&value, sizeof(value));
    }

    template <typename T>
    T Receive() const {
        T value{};
        RecvRaw(&value, sizeof(value));
        return value;
    }

private:
    void SendRaw(std::byte const* buffer, std::size_t byte_size) const;
    void RecvRaw(std::byte* buffer, std::size_t byte_size) const;

    int fd_ = -1;
};

AbstractSocket Listen(std::string_view name, int backlog = -1);
AbstractSocket Connect(std::string_view name);
}  // namespace messenger::system
