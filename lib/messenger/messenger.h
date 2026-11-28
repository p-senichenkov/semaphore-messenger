#pragma once

#include <memory>
#include <string>
#include <unistd.h>
#include <utility>

#include "lib/handshake/handshake.h"
#include "lib/messenger/message_queue.h"
#include "lib/resources/holder.h"
#include "lib/util/ensure.h"

namespace messenger {
class IMessenger {
public:
    virtual ~IMessenger() = default;

    virtual void Write(std::string&& msg) = 0;
    virtual std::string WaitRead() = 0;

    // I don't want to manage statuses, so just provide a separate function
    virtual void Handshake() = 0;
};

class MessengerBase : public IMessenger {
public:
    void Write(std::string&& msg) final {
        ENSURE(message_queue_);
        message_queue_->Write(std::move(msg));
    }

    std::string WaitRead() final {
        ENSURE(message_queue_);
        return message_queue_->WaitRead();
    }

    void Handshake() final {
        message_queue_ = std::make_unique<MessageQueue>(HandshakeImpl());
        message_queue_->Start();
    }

protected:
    virtual std::unique_ptr<IIpcResources> HandshakeImpl() const = 0;

private:
    std::unique_ptr<MessageQueue> message_queue_;
};

class MasterMessenger final : public MessengerBase {
public:
    explicit MasterMessenger(pid_t pid) : pid_(pid) {}

    std::unique_ptr<IIpcResources> HandshakeImpl() const override {
        return handshake::DoMasterHandshake(pid_);
    }

private:
    pid_t pid_;
};

class SlaveMessenger final : public MessengerBase {
public:
    explicit SlaveMessenger(pid_t master_pid) : master_pid_(master_pid) {}

    std::unique_ptr<IIpcResources> HandshakeImpl() const override {
        return handshake::DoSlaveHandshake(master_pid_);
    }

private:
    pid_t master_pid_;
};
}  // namespace messenger
