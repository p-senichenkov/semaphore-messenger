#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <utility>

#include "lib/messenger/messenger.h"
#include "tests/assert.h"

namespace tests {
class MockedMessengerBase {
public:
    virtual ~MockedMessengerBase() = default;

    explicit MockedMessengerBase(std::unique_ptr<messenger::IMessenger>&& messenger)
        : messenger_(std::move(messenger)) {
        ASSERT(messenger_);
        messenger_->Handshake();
    }

    MockedMessengerBase& Send(std::string&& message) {
        ASSERT(messenger_);
        messenger_->Write(std::move(message));
        return *this;
    }

    MockedMessengerBase& Receive(std::string const& message) {
        ASSERT(messenger_);
        auto msg = messenger_->WaitRead();
        ASSERT(msg == message);
        return *this;
    }

    template <typename Rep, typename Period>
    MockedMessengerBase& Sleep(std::chrono::duration<Rep, Period>&& sleep_time) {
        std::this_thread::sleep_for(sleep_time);
    }

private:
    std::unique_ptr<messenger::IMessenger> messenger_;
};

class MasterMockedMessenger final : public MockedMessengerBase {
public:
    MasterMockedMessenger()
        : MockedMessengerBase(std::make_unique<messenger::MasterMessenger>(getpid())) {}
};

class SlaveMockedMessenger final : public MockedMessengerBase {
public:
    explicit SlaveMockedMessenger(pid_t master_pid)
        : MockedMessengerBase(std::make_unique<messenger::SlaveMessenger>(master_pid)) {}
};
}  // namespace tests
