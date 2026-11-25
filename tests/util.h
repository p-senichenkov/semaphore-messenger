#pragma once

#include <functional>
#include <gtest/gtest.h>
#include <unistd.h>

namespace tests {
void ForkTest(std::function<void(pid_t)> parent_work, std::function<void(pid_t)> child_work) {
    ASSERT_TRUE(parent_work);
    ASSERT_TRUE(child_work);

    int f = fork();
    ASSERT_GE(f, 0) << "Fork failed";
    if (f > 0) {
        parent_work(f);
    } else {
        child_work(getppid());
    }
}
}  // namespace tests
