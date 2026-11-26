#include "message_queue.h"

#include <thread>
#include <utility>

using namespace messenger;

void MessageQueue::Start() {
    writing_thread_ = std::jthread{[this]() { WriteMessages(); }};
    reading_thread_ = std::jthread{[this]() { ReadMessages(); }};
}

void MessageQueue::ReadMessages() {
    while (working_) {
        auto msg = message_io_.Read();
        read_queue_.Push(std::move(msg));
        if (on_read_queue_size_change_) {
            on_read_queue_size_change_(read_queue_.Size());
        }
    }
}

void MessageQueue::WriteMessages() {
    while (working_) {
        auto msg = write_queue_.WaitPop();
        if (on_write_queue_size_change_) {
            on_write_queue_size_change_(write_queue_.Size());
        }
        message_io_.Write(std::move(msg));
    }
}
