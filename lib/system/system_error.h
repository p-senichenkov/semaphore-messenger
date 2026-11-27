#pragma once

#include <cerrno>
#include <format>
#include <string>
#include <system_error>
#include <unistd.h>

namespace messenger::system {
class SystemError : public std::system_error {
public:
    SystemError(int err_code, std::string const& msg)
        : std::system_error(err_code, std::generic_category(),
                            std::format("(PID {}) {}", getpid(), msg)) {}

    explicit SystemError(std::string const& msg) : SystemError(errno, msg) {}
};
}  // namespace messenger::system
