#include "lib/messenger/message_io.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <string_view>

#include "lib/messages/control_area.h"
#include "lib/messages/user_text.h"
#include "lib/util/bit_flags.h"
#include "lib/util/log.h"

using namespace messenger;

namespace {
constexpr static bool kDebugCleanReadMessages = true;
constexpr static std::string_view kCleanMsgText = "DEADBEEF";
}  // namespace

void MessageIO::Write(std::string&& message) {
    if (message.empty()) {
        throw WriteError{"Empty message"};
    }
    if (message.size() > kMaxPayloadSizeBytes) {
        throw WriteError{"Message is too long"};
    }

    UserTextMsg user_text_msg;
    std::memcpy(user_text_msg.payload, message.c_str(), message.size() + 1);

    ipc_resources_->OurWriteSemaphore().Dec();

    auto& their_msg_place = ipc_resources_->TheirMsgPlace();
    their_msg_place = std::move(user_text_msg);

    ipc_resources_->TheirReadSemaphore().Inc();
}

bool MessageIO::CanWriteImmediately() const {
    return ipc_resources_->TheirWriteSemaphore().CanDecImmediately();
}

std::string MessageIO::Read() {
    ipc_resources_->OurReadSemaphore().Dec();

    if (util::IsFlagSet(ipc_resources_->ControlArea(), EControlArea::kShutdown)) {
        // TODO: Even this message doesn't appear...
        LOG(Info) << "Exit control byte is set!";
        throw ControlExit{};
    }

    auto const& payload = ipc_resources_->OurMsgPlace().payload;
    std::string message{payload, std::ranges::find(payload, '\0')};
    if constexpr (kDebugCleanReadMessages) {
        std::memcpy(ipc_resources_->OurMsgPlace().payload, kCleanMsgText.data(),
                    kCleanMsgText.size() + 1);
    }
    ipc_resources_->TheirWriteSemaphore().Inc();

    return message;
}

bool MessageIO::CanReadImmediately() const {
    return ipc_resources_->OurReadSemaphore().CanDecImmediately();
}

void MessageIO::SendControl(EControlArea control) {
    // TODO: Take OR here?
    ipc_resources_->ControlArea() = control;
    ipc_resources_->TheirReadSemaphore().Inc();
}
