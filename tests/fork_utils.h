#pragma once

#include <cerrno>
#include <functional>
#include <string>
#include <unistd.h>

#include <sys/wait.h>

#include "tests/assert.h"

namespace tests {
void ForkTest(std::function<void(pid_t)> parent_work, std::function<void(pid_t)> child_work) {
    ASSERT(parent_work);
    ASSERT(child_work);

    int pid = fork();
    ASSERT(pid >= 0);
    if (pid > 0) {
        parent_work(pid);

        int status;
        pid_t result;
        do {
            result = waitpid(pid, &status, 0);
        } while (result == -1 && errno == EINTR);
        ASSERT(result == pid);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);
    } else {
        child_work(getppid());
        _exit(0);
    }
}
}  // namespace tests
