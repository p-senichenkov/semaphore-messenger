#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "lib/core/messages/control_area.h"
#include "lib/core/resources/holder.h"

namespace messenger {
// WARN: Calling a single function from multiple threads *is not* safe
// Calling different functions concurrently *is* safe
class MessageIO {
public:
    // An attempt to write a bad message
    struct WriteError : public std::runtime_error {
        explicit WriteError(std::string&& msg) : std::runtime_error(msg) {}
    };

    // TODO: Constructor
    // TODO: Something else?

    void Write(std::string&& message);
    bool CanWriteImmediately() const;

    std::string Read();
    bool CanReadImmediately() const;

    void SendControl(EControlArea control) {
        // TODO:
    }

private:
    std::unique_ptr<IIpcResources> ipc_resources_;
};
}  // namespace messenger
