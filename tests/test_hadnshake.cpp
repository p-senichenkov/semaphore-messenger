#include <gtest/gtest.h>
#include <unistd.h>

#include <sys/types.h>

#include "lib/handshake/handshake.h"
#include "tests/util.h"

namespace tests {
TEST(TestHandshake, TestHandshakeSucceeds) {
    auto master_work = [](pid_t) {
        auto resources = messenger::handshake::DoMasterHandshake(getpid());
        ASSERT_TRUE(resources);
    };
    auto slave_work = [](pid_t master_pid) {
        auto resources = messenger::handshake::DoSlaveHandshake(master_pid);
        ASSERT_TRUE(resources);
    };
    ForkTest(master_work, slave_work);
}
}  // namespace tests
