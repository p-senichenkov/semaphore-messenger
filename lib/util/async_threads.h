#include <chrono>
#include <functional>
#include <future>
#include <initializer_list>
#include <stop_token>
#include <thread>
#include <vector>

namespace messenger::util {
/// @brief Run multiple threads without losing exceptions from them
/// Also provides convenient cancellation
class AsyncThreads {
public:
    using Task = std::function<void(std::stop_token)>;

    AsyncThreads(std::initializer_list<Task>&& tasks);

    AsyncThreads(AsyncThreads const&) = delete;
    AsyncThreads& operator=(AsyncThreads const&) = delete;
    AsyncThreads(AsyncThreads&&) = delete;
    AsyncThreads& operator=(AsyncThreads&&) = delete;

    void CheckException() noexcept(false);

    void Stop();

private:
    /// @return true -- must stop
    bool CheckThreads();

    constexpr static auto kCheckTimeout = std::chrono::milliseconds{10};
    constexpr static auto kCheckPeriod = std::chrono::milliseconds{500};

    std::vector<std::jthread> threads_;
    std::vector<std::future<void>> futures_;
    std::jthread checking_thread_;

    // Could use std::atomic<std::exception_ptr>, but exception_ptr is not trivially copyable
    std::promise<void> promise_;
    std::future<void> future_;
};
}  // namespace messenger::util
