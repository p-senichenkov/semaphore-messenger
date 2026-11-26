#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <utility>

namespace messenger::util {
template <typename Val>
class AsyncQueue {
public:
    Val WaitPop() {
        std::unique_lock lk{mutex_};
        not_empty_cv_.wait(lk, [this]() { return !queue_.empty(); });
        Val val = queue_.front();
        queue_.pop();
        return val;
    }

    void Push(Val&& val) {
        {
            std::lock_guard guard{mutex_};
            queue_.push(std::forward<Val>(val));
        }
        not_empty_cv_.notify_one();
    }

    std::size_t Size() const {
        std::lock_guard guard{mutex_};
        return queue_.size();
    }

private:
    std::queue<Val> queue_;

    mutable std::mutex mutex_;
    std::condition_variable not_empty_cv_;
};
}  // namespace messenger::util
