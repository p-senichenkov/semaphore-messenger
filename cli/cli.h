#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <utility>

#include <sys/types.h>

#include "lib/messenger/messenger.h"
#include "lib/util/ensure.h"

namespace cli {
struct CliParams {
    std::string_view executable_name;
    std::optional<pid_t> peer_pid = std::nullopt;
};

class CliBase {
public:
    explicit CliBase(CliParams&& params) : params_(std::move(params)) {}

    virtual ~CliBase() = default;

    void Work() {
        messenger_ = Init();
        ENSURE(messenger_);
        std::cout << "Performing handshake...\n";
        messenger_->Handshake();
        std::cout << "Handshake successful\n";
        MainLoop();
    }

protected:
    virtual std::unique_ptr<messenger::IMessenger> Init() = 0;

    CliParams const& Params() const noexcept {
        return params_;
    }

private:
    void MainLoop();

    void Listen() {
        while (working_) {
            // TODO: Exceptions (especially ExitFlag) won't propagate here
            auto msg = messenger_->WaitRead();
            std::cout << "Got message: " << msg << '\n';
        }
    }

    CliParams params_;

    std::unique_ptr<messenger::IMessenger> messenger_;

    std::jthread listening_thread_;
    bool working_ = true;
};

class MasterCli final : public CliBase {
public:
    explicit MasterCli(CliParams&& params) : CliBase(std::move(params)) {}

    std::unique_ptr<messenger::IMessenger> Init() override;
};

class SlaveCli final : public CliBase {
public:
    explicit SlaveCli(CliParams&& params) : CliBase(std::move(params)) {}

    std::unique_ptr<messenger::IMessenger> Init() override;
};

void RunCli(int argc, char const** argv);
}  // namespace cli
