#pragma once

#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <initializer_list>
#include <vector>

#include "lib/util/ensure.h"

namespace messenger::util {
class AsyncWhile {
public:
    using Task = std::function<void()>;

    explicit AsyncWhile(std::initializer_list<Task>&& tasks) : tasks_(tasks.begin(), tasks.end()) {
        ENSURE(tasks.size() > 0);
        Run();
    }

    AsyncWhile(AsyncWhile const&) = delete;
    AsyncWhile& operator=(AsyncWhile const&) = delete;
    AsyncWhile(AsyncWhile&&) = delete;
    AsyncWhile& operator=(AsyncWhile&&) = delete;

    ~AsyncWhile() {
        working_.clear(std::memory_order_relaxed);
    }

    void Run() {
        using namespace std::chrono_literals;

        working_.test_and_set(std::memory_order_relaxed);
        std::vector<std::future<void>> futures;
        for (auto& task : tasks_) {
            futures.push_back(std::async(std::launch::async, [this, &task]() {
                // TODO: Cancellation?
                while (working_.test(std::memory_order_relaxed)) {
                    try {
                        task();
                    } catch (std::exception const&) {
                        working_.clear(std::memory_order_relaxed);
                        working_.notify_all();
                        throw;
                    }
                }
            }));
        }

        working_.wait(true, std::memory_order_relaxed);

        for (auto& future : futures) {
            if (future.wait_for(10ms) == std::future_status::ready) {
                // Should throw (or return void if exited by seeing working_ clean)
                future.get();
            }
        }
        ENSURE_MSG(false, "Working is false, but none of futures thrown");
    }

private:
    constexpr static auto kCheckPeriod = std::chrono::milliseconds{100};

    std::vector<Task> tasks_;
    std::atomic_flag working_ = ATOMIC_FLAG_INIT;
};
}  // namespace messenger::util
