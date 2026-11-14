#pragma once

#include <cerrno>
#include <string>
#include <system_error>

namespace messenger::system {
class SystemError : public std::system_error {
public:
    SystemError(int err_code, std::string const& msg)
        : std::system_error(err_code, std::generic_category(), msg) {}

    explicit SystemError(std::string const& msg) : SystemError(errno, msg) {}
};
}  // namespace messenger::system
