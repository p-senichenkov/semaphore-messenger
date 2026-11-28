#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <stop_token>
#include <string>
#include <utility>

#include "lib/messages/control_area.h"
#include "lib/messenger/message_io.h"
#include "lib/resources/holder.h"
#include "lib/util/async_queue.h"
#include "lib/util/async_threads.h"
#include "lib/util/ensure.h"

namespace messenger {
class MessageQueue {
public:
    using QueueSizeChangeCallback = std::function<void(std::size_t)>;

    // NOTE: Callbacks must be fast -- they are on critical path!
    explicit MessageQueue(std::unique_ptr<IIpcResources>&& resources,
                          QueueSizeChangeCallback&& on_read_queue_size_change = nullptr,
                          QueueSizeChangeCallback&& on_write_queue_size_change = nullptr)
        : message_io_(std::move(resources)),
          on_read_queue_size_change_(on_read_queue_size_change),
          on_write_queue_size_change_(on_write_queue_size_change),
          threads_(std::make_unique<util::AsyncThreads>(
                  std::initializer_list<util::AsyncThreads::Task>{
                          [this](std::stop_token token) { WriteMessages(token); },
                          [this](std::stop_token token) { ReadMessages(token); }})) {}

    ~MessageQueue() {
        Stop();
    }

    std::size_t ReadQueueSize() const {
        return read_queue_.Size();
    }

    std::size_t WriteQueueSize() const {
        return write_queue_.Size();
    }

    void Write(std::string&& msg) {
        write_queue_.Push(std::move(msg));
    }

    std::string WaitRead() {
        auto msg = read_queue_.WaitPop();
        return msg;
    }

    void CheckException() noexcept(false) {
        ENSURE(threads_);
        threads_->CheckException();
    }

    void Stop() {
        message_io_.SendControl(EControlArea::kShutdown);
        // TODO: Cancellation?
    }

private:
    void ReadMessages(std::stop_token token);
    void WriteMessages(std::stop_token token);

    MessageIO message_io_;

    std::unique_ptr<util::AsyncThreads> threads_;

    util::AsyncQueue<std::string> write_queue_;
    util::AsyncQueue<std::string> read_queue_;

    QueueSizeChangeCallback on_read_queue_size_change_;
    QueueSizeChangeCallback on_write_queue_size_change_;
};
}  // namespace messenger
