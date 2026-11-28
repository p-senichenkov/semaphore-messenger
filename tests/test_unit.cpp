#include <cstddef>
#include <exception>
#include <format>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>

#include "lib/util/async_threads.h"
#include "lib/util/log.h"
#include "tests/assert.h"

using namespace tests;
using namespace std::chrono_literals;

struct ExceptionFromThread : public std::runtime_error {
    ExceptionFromThread() : std::runtime_error("Exception from thread") {}
};

void Work(unsigned short thread_id, unsigned short steps_before_throw, bool exit_expected,
          std::stop_token token) {
    auto debug = [thread_id](std::string&& msg) {
        LOG(Debug) << "[Thread " << thread_id << "] " << std::move(msg);
    };
    for (unsigned i = 0; i < steps_before_throw; ++i) {
        if (token.stop_requested()) {
            debug("Stopped gracefully");
            return;
        }
        debug("Tick");
        std::this_thread::sleep_for(200ms);
    }
    if (exit_expected) {
        throw ExceptionFromThread{};
    }
    throw std::runtime_error{std::format("Thread {} unexpectedly made to the end", thread_id)};
}

auto MakeWork(unsigned short thread_id, unsigned short steps_before_throw, bool exit_expected) {
    return [thread_id, steps_before_throw, exit_expected](std::stop_token token) {
        Work(thread_id, steps_before_throw, exit_expected, token);
    };
}

MSG_TEST(AsyncThreads) {
    AssertThrows<ExceptionFromThread>([]() {
        messenger::util::AsyncThreads threads{MakeWork(1, 2, true), MakeWork(2, 6, false),
                                              MakeWork(3, 10, false), MakeWork(4, 14, false)};

        for (std::size_t i = 0; i < 10; ++i) {
            LOG(Debug) << "Waiting...";
            std::this_thread::sleep_for(300ms);
            threads.CheckException();
        }
        throw std::runtime_error{"Main worker made to the end without exceptions"};
    });
}

MSG_TEST(AsyncThreadsCancel) {
    try {
        messenger::util::AsyncThreads threads{MakeWork(1, 15, false), MakeWork(2, 15, false)};

        for (std::size_t i = 0; i < 3; ++i) {
            LOG(Debug) << "Waiting...";
            std::this_thread::sleep_for(300ms);
            threads.CheckException();
        }
        threads.Stop();
        std::this_thread::sleep_for(500ms);
        threads.CheckException();
    } catch (std::exception const& exc) {
        Fail(exc.what());
    }
}

int main() {
    MSG_RUN_TEST(AsyncThreads);
    MSG_RUN_TEST(AsyncThreadsCancel);
}
