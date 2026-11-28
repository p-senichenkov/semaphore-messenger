#include "message_queue.h"

#include <stop_token>
#include <utility>

using namespace messenger;

void MessageQueue::ReadMessages(std::stop_token token) {
    while (!token.stop_requested()) {
        auto msg = message_io_.Read();
        read_queue_.Push(std::move(msg));
        if (on_read_queue_size_change_) {
            on_read_queue_size_change_(read_queue_.Size());
        }
    }
}

void MessageQueue::WriteMessages(std::stop_token token) {
    while (!token.stop_requested()) {
        auto msg = write_queue_.WaitPop();
        if (on_write_queue_size_change_) {
            on_write_queue_size_change_(write_queue_.Size());
        }
        message_io_.Write(std::move(msg));
    }
}
