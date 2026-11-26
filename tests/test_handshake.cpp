#include <unistd.h>

#include <sys/types.h>

#include "lib/handshake/handshake.h"
#include "tests/assert.h"
#include "tests/fork_utils.h"

using namespace tests;

MSG_TEST(HandshakeSucceeds) {
    tests::ForkTest(
            [](pid_t) {
                auto resources = messenger::handshake::DoMasterHandshake(getpid());
                ASSERT(resources);
            },
            [](pid_t parent_pid) {
                auto resources = messenger::handshake::DoSlaveHandshake(parent_pid);
                ASSERT(resources);
            });
}

int main() {
    MSG_RUN_TEST(HandshakeSucceeds);
}
