#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "lib/core/messenger/message_io.h"
#include "lib/core/resources/holder.h"
#include "lib/core/util/async_queue.h"
#include "lib/core/util/ensure.h"

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
          on_write_queue_size_change_(on_write_queue_size_change) {}

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
        ENSURE_MSG(writing_thread_.joinable(), "Start() must be called before Write()");
        write_queue_.Push(std::move(msg));
    }

    std::string WaitRead() {
        ENSURE_MSG(reading_thread_.joinable(), "Start() must be called before Write()");
        return read_queue_.WaitPop();
    }

    void Start();

    // TODO: Do we need this? Should stop well in destructor
    void Stop() {
        working_ = false;
    }

private:
    void ReadMessages();
    void WriteMessages();

    MessageIO message_io_;

    util::AsyncQueue<std::string> write_queue_;
    util::AsyncQueue<std::string> read_queue_;

    std::jthread reading_thread_;
    std::jthread writing_thread_;
    bool working_ = true;

    QueueSizeChangeCallback on_read_queue_size_change_;
    QueueSizeChangeCallback on_write_queue_size_change_;
};
}  // namespace messenger
