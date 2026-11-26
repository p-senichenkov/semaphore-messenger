#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace messenger {
// We rely heavily on exception mechanism, so don't really want to use C assertions
struct AssertionError : public std::runtime_error {
    explicit AssertionError(std::string&& msg) : std::runtime_error(std::move(msg)) {}
};
}  // namespace messenger

#define ENSURE_MSG(EXPR, MSG)                     \
    if (!(EXPR)) {                                \
        throw ::messenger::AssertionError{(MSG)}; \
    }

#define ENSURE(EXPR) ENSURE_MSG(EXPR, "Condition violated: " #EXPR)
