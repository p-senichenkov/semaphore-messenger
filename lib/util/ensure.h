#pragma once

#include <format>
#include <source_location>
#include <stdexcept>
#include <string>
#include <utility>

namespace messenger {
// We rely heavily on exception mechanism, so don't really want to use C assertions
struct AssertionError : public std::runtime_error {
    explicit AssertionError(std::string&& msg,
                            std::source_location&& loc = std::source_location::current())
        : std::runtime_error(std::format("{}:{} {}: {}", loc.file_name(), loc.line(),
                                         loc.function_name(), std::move(msg))) {}
};
}  // namespace messenger

#define ENSURE_MSG(EXPR, MSG) \
    if (!(EXPR)) throw ::messenger::AssertionError{(MSG)};

#define ENSURE(EXPR) ENSURE_MSG(EXPR, "Condition violated: " #EXPR)
