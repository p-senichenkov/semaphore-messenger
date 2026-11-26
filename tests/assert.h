#pragma once

#include <cstdlib>
#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <unistd.h>
#include <utility>

namespace tests::detail {
void Check(bool condition, std::string&& condition_str,
           std::source_location loc = std::source_location::current()) {
    if (!condition) {
        std::cerr << std::format("[PID {}] {}:{}: Condition violated: {}\n", getpid(),
                                 loc.file_name(), loc.line(), std::move(condition_str));

        std::abort();
    }
}
}  // namespace tests::detail

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
