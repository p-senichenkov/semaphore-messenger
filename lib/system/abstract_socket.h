#pragma once

#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

#include <sys/socket.h>

#include "lib/system/system_error.h"
#include "lib/util/ensure.h"

namespace messenger::system {

/// @brief An owning handle to an abstract UNIX socket
class AbstractSocket {
public:
    explicit AbstractSocket(int fd) noexcept : fd_(fd) {}

    AbstractSocket(AbstractSocket const&) = delete;
    AbstractSocket& operator=(AbstractSocket const&) = delete;

    AbstractSocket(AbstractSocket&& other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

    AbstractSocket& operator=(AbstractSocket&& other) noexcept {
        AbstractSocket tmp{std::move(other)};
        Swap(tmp);
        return *this;
    }

    ~AbstractSocket();

    int Fd() const noexcept {
        return fd_;
    }

    AbstractSocket Accept(std::string_view name) const;

    template <typename T>
    void Send(T const& value) const {
        ENSURE(fd_ != -1);

        auto const n = send(fd_, &value, sizeof(value), MSG_NOSIGNAL);
        if (n == -1) {
            throw SystemError{"send"};
        }
        ENSURE(static_cast<std::size_t>(n) == sizeof(value));
    }

    template <typename T>
        requires std::is_default_constructible_v<T>
    T Receive() const {
        ENSURE(fd_ != -1);

        T value;

        auto const n = recv(fd_, &value, sizeof(value), MSG_TRUNC);
        if (n == -1) {
            throw SystemError{"recv"};
        }
        if (n == 0) {
            throw std::runtime_error("Abstract socket Receive: peer disconneceted");
        }
        ENSURE(static_cast<std::size_t>(n) == sizeof(value));

        return value;
    }

private:
    void Swap(AbstractSocket& other) {
        std::swap(fd_, other.fd_);
    }

    int fd_ = -1;
};

AbstractSocket Listen(std::string_view name);
AbstractSocket Connect(std::string_view name);
}  // namespace messenger::system
