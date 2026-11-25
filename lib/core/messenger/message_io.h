#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include "lib/core/messages/control_area.h"
#include "lib/core/resources/holder.h"

namespace messenger {
struct ControlExit : public std::runtime_error {
    ControlExit() : std::runtime_error("Exit byte is set") {}
};

// WARN: Calling a single function from multiple threads *is not* safe
// Calling different functions concurrently *is* safe
class MessageIO {
public:
    // An attempt to write a bad message
    struct WriteError : public std::runtime_error {
        explicit WriteError(std::string&& msg) : std::runtime_error(msg) {}
    };

    explicit MessageIO(std::unique_ptr<IIpcResources>&& resources)
        : ipc_resources_(std::move(resources)) {}

    void Write(std::string&& message);
    bool CanWriteImmediately() const;

    /// @throws ControlExit
    std::string Read();
    // TODO: Most likely not needed, remove
    bool CanReadImmediately() const;

    void SendControl(EControlArea control);

private:
    std::unique_ptr<IIpcResources> ipc_resources_;
};
}  // namespace messenger
