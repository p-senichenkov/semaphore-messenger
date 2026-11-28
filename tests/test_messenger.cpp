#include <iostream>

#include <sys/shm.h>

#include "lib/messenger/message_io.h"
#include "tests/assert.h"
#include "tests/fork_utils.h"
#include "tests/mocks.h"
using namespace tests;

MSG_TEST(SimpleMessagePassing) {
    ForkTest(
            [](pid_t) {
                try {
                    AssertThrows<messenger::ControlExit>(
                            []() { MasterMockedMessenger{}.Send("Hello").Receive("Hi"); });
                } catch (...) {
                    std::cout << "Caught!\n";
                }
            },
            [](pid_t parent_pid) {
                try {
                    AssertThrows<messenger::ControlExit>([parent_pid]() {
                        SlaveMockedMessenger{parent_pid}.Receive("Hello").Send("Hi");
                    });
                } catch (...) {
                    std::cout << "Caught!\n";
                }
            });
}

int main() {
    MSG_RUN_TEST(SimpleMessagePassing);
}
