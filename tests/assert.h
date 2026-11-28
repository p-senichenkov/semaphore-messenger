#pragma once

#include <concepts>
#include <cstdlib>
#include <exception>
#include <format>
#include <functional>
#include <iostream>
#include <source_location>
#include <string>
#include <unistd.h>
#include <utility>

namespace tests {
void Fail(std::string&& msg, std::source_location loc = std::source_location::current()) {
    std::cerr << std::format("[PID {}] {}:{}: {}\n", getpid(), loc.file_name(), loc.line(),
                             std::move(msg));
    std::abort();
}

namespace detail {
void Check(bool condition, std::string&& condition_str,
           std::source_location loc = std::source_location::current()) {
    if (!condition) {
        Fail(std::format("Condition violated: {}", std::move(condition_str)), loc);
    }
}
}  // namespace detail

template <std::derived_from<std::exception> Exc>
void AssertThrows(std::function<void()>&& body) {
    try {
        body();
        Fail("Did not throw any exception");
    } catch (Exc const&) {
        // Succeed
    } catch (std::exception const& e) {
        // Cannot simply re-throw here...
        Fail(std::format("e.what()"));
    }
}
}  // namespace tests

#define ASSERT(EXPR) ::tests::detail::Check(!!(EXPR), #EXPR)

#define MSG_PRIVATE_TEST_FUNC_NAME(NAME) NAME##_main
#define MSG_PRIVATE_TEST_BODY_NAME(NAME) NAME##_body

#define MSG_TEST(NAME)                        \
    void MSG_PRIVATE_TEST_BODY_NAME(NAME)();  \
    void MSG_PRIVATE_TEST_FUNC_NAME(NAME)() { \
        std::cout << "[RUN " #NAME "]\n";     \
        MSG_PRIVATE_TEST_BODY_NAME(NAME)();   \
        std::cout << "[SUCCESS " #NAME "]\n"; \
    }                                         \
                                              \
    void MSG_PRIVATE_TEST_BODY_NAME(NAME)()

#define MSG_RUN_TEST(NAME) MSG_PRIVATE_TEST_FUNC_NAME(NAME)()
