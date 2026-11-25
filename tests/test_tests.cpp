#include <ctime>
#include <gtest/gtest.h>

#include "util.h"

namespace tests {
TEST(TestTests, TestFork) {
    ForkTest([](pid_t child_pid) { ASSERT_GT(child_pid, 0); },
             [](pid_t parent_pid) { ASSERT_GT(parent_pid, 0); });
}
}  // namespace tests
