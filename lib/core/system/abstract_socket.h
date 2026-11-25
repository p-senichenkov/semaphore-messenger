#pragma once

#include <cstddef>
#include <string_view>
#include <type_traits>

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
        auto const* bytes = reinterpret_cast<std::byte const*>(&value);
        SendRaw(bytes, sizeof(T));
    }

    template <typename T>
        requires std::is_default_constructible_v<T>
    T Receive() const {
        T value;
        RecvRaw(reinterpret_cast<std::byte*>(&value), sizeof(T));
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
