#include "lib/util/async_threads.h"

#include <initializer_list>
#include <stop_token>

#include "lib/util/ensure.h"

using namespace messenger::util;

AsyncThreads::AsyncThreads(std::initializer_list<Task>&& tasks) : future_(promise_.get_future()) {
    ENSURE(tasks.size() > 0);

    threads_.reserve(tasks.size());
    futures_.reserve(tasks.size());
    for (auto& task : tasks) {
        std::packaged_task pt{task};
        futures_.push_back(pt.get_future());
        threads_.push_back(std::jthread{std::move(pt)});
    }

    checking_thread_ = std::jthread{[this](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(kCheckPeriod);
            if (CheckThreads()) {
                break;
            }
        }
    }};
}

void AsyncThreads::CheckException() {
    if (!future_.valid()) {
        return;
    }
    if (future_.wait_for(kCheckTimeout) == std::future_status::ready) {
        future_.get();
    }
}

void AsyncThreads::Stop() {
    for (auto& thread : threads_) {
        thread.request_stop();
    }
    checking_thread_.request_stop();
}

bool AsyncThreads::CheckThreads() {
    for (auto& future : futures_) {
        if (!future.valid()) {
            continue;
        }
        if (future.wait_for(kCheckTimeout) == std::future_status::ready) {
            try {
                future.get();
            } catch (std::exception const&) {
                promise_.set_exception(std::current_exception());
                Stop();
                return true;
            }
        }
    }
    return false;
}
