#pragma once

#include <cerrno>
#include <format>
#include <string>
#include <system_error>
#include <unistd.h>

namespace messenger::system {
class SystemError : public std::system_error {
public:
    explicit SystemError(std::string const& msg)
        : std::system_error(errno, std::generic_category(),
                            std::format("(PID {}) {}", getpid(), msg)) {}
};
}  // namespace messenger::system
