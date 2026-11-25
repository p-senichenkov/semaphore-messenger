#include "cli.h"

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include <sys/types.h>

#include "lib/core/util/ensure.h"

namespace {
struct ParsingError : public std::runtime_error {
    ParsingError(std::string const& value, std::string const& name, std::string const& what)
        : std::runtime_error("Error parsing value '" + value + "' for argument " + name.data() +
                             ": " + what) {}
};

template <typename T>
    requires std::is_default_constructible_v<T>
T ParseParam(std::string_view param, std::string_view name) {
    try {
        std::istringstream iss{std::string{param}};
        T value;
        iss >> value;
        return value;
    } catch (std::exception const& exc) {
        throw ParsingError{std::string{param}, std::string{name}, exc.what()};
    }
}

template <>
std::string_view ParseParam<std::string_view>(std::string_view param, std::string_view) {
    return param;
}

void ShowHelp(std::string_view const& executable_name) {
    std::cout << "Messenger CLI\n";
    std::cout << "\tRun master process: `" << executable_name << "`\n";
    std::cout << "\tRun slave process: `" << executable_name << " <master PID>`\n";
}

std::optional<cli::CliParams> ParseParams(int argc, char const** argv) {
    using namespace std::string_literals;

    auto executable_name = ParseParam<std::string_view>(argv[0], "executable name");

    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};
        if (arg == "-h" || arg == "--help") {
            ShowHelp(executable_name);
            return std::nullopt;
        }
    }

    if (argc == 1) {
        return cli::CliParams{.executable_name = executable_name};
    }
    if (argc >= 2) {
        if (argc > 2) {
            std::ostringstream extra_params;
            for (int i = 2; i < argc; ++i) {
                extra_params << argv[i];
                if (i != argc - 1) {
                    extra_params << ", ";
                }
                std::cout << "Warning: ignoring extra arguments: " << extra_params.str();
            }
        }

        auto peer_pid = ParseParam<pid_t>(argv[1], "peer PID");
        return cli::CliParams{.executable_name = executable_name, .peer_pid = peer_pid};
    }
    ENSURE_MSG(false, "Unreachable");
}
}  // namespace

namespace cli {
void CliBase::MainLoop() {
    std::cout << "Starting listening thread...\n";
    listening_thread_ = std::jthread{[this]() { Listen(); }};
    std::cout << "Ready to send messages. Ctrl+D to stop\n";
    while (working_) {
        std::string line;
        std::getline(std::cin, line);
        if (!std::cin) {
            working_ = false;
            break;
        }
        messenger_->Write(std::move(line));
    }
    std::cout << "Stopping...\n";
}

std::unique_ptr<messenger::IMessenger> MasterCli::Init() {
    pid_t pid = getpid();
    auto messenger = std::make_unique<messenger::MasterMessenger>(pid);
    std::cout << "Ready to accept peer, our PID is " << pid << '\n';
    std::cout << "Start peer with `" << Params().executable_name << ' ' << pid << "`\n";
    return messenger;
}

std::unique_ptr<messenger::IMessenger> SlaveCli::Init() {
    ENSURE(Params().peer_pid);
    auto const pid = *Params().peer_pid;
    std::cout << "Connecting to peer at PID " << pid << '\n';
    return std::make_unique<messenger::SlaveMessenger>(pid);
}

void RunCli(int argc, char const** argv) {
    auto maybe_params = ParseParams(argc, argv);
    if (!maybe_params) {
        return;
    }
    CliParams& params = *maybe_params;

    std::unique_ptr<CliBase> cli;
    if (params.peer_pid) {
        cli = std::make_unique<SlaveCli>(std::move(params));
    } else {
        cli = std::make_unique<MasterCli>(std::move(params));
    }

    cli->Work();
    std::cout << "Bye!\n";
}
}  // namespace cli
