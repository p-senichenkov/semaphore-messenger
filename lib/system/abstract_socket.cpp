#include "lib/system/abstract_socket.h"

#include <cstddef>
#include <string_view>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "lib/system/system_error.h"
#include "lib/util/ensure.h"

namespace messenger::system {
namespace {
AbstractSocket MakeSocket() {
    int const fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);

    if (fd == -1) {
        throw SystemError{"socket"};
    }
    return AbstractSocket{fd};
}

std::pair<sockaddr_un, socklen_t> GetAddress(std::string_view name) {
    sockaddr_un addr{};

    if (name.empty() || name.size() > sizeof(addr.sun_path) - 1 ||
        name.find('\0') != std::string_view::npos)
        throw std::invalid_argument("invalid abstract socket name");

    addr.sun_family = AF_UNIX;
    addr.sun_path[0] = '\0';

    std::memcpy(addr.sun_path + 1, name.data(), name.size());

    return {addr, static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) + 1 + name.size())};
}
}  // namespace

AbstractSocket AbstractSocket::Accept(std::string_view name) const {
    ENSURE(fd_ != -1);
    int fd;
    do {
        // TODO: Timeout here
        fd = accept4(fd_, nullptr, nullptr, SOCK_CLOEXEC);
    } while (fd == -1 && errno == EINTR);

    if (fd == -1) {
        throw SystemError{"accept4"};
    }
    return AbstractSocket{fd};
}

void AbstractSocket::Close() {
    if (fd_ >= 0) {
        close(fd_);
    }
    fd_ = -1;
}

AbstractSocket Listen(std::string_view name) {
    auto const [addr, len] = GetAddress(name);
    AbstractSocket sock = MakeSocket();

    if (bind(sock.Fd(), reinterpret_cast<sockaddr const*>(&addr), len) == -1) {
        throw SystemError{"bind"};
    }

    if (listen(sock.Fd(), 1) == -1) {
        throw SystemError{"listen"};
    }

    return sock;
}

AbstractSocket Connect(std::string_view name) {
    auto const [addr, len] = GetAddress(name);
    AbstractSocket sock = MakeSocket();

    while (connect(sock.Fd(), reinterpret_cast<sockaddr const*>(&addr), len) == -1) {
        if (errno == EINTR) {
            continue;
        }
        throw SystemError{"connect"};
    }

    return sock;
}
}  // namespace messenger::system
