#include "lib/core/messenger/message_io.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "lib/core/messages/user_text.h"

using namespace messenger;

void MessageIO::Write(std::string&& message) {
    if (message.empty()) {
        throw WriteError{"Empty message"};
    }
    if (message.size() > kMaxPayloadSize) {
        throw WriteError{"Message is too long"};
    }

    UserTextMsg user_text_msg;
    std::memcpy(user_text_msg.payload, message.c_str(), message.size() + 1);

    // TODO: Statuses (here and everywhere)
    ipc_resources_->TheirWriteSemaphore().Dec();

    // TODO: Check control area

    auto& their_msg_place = ipc_resources_->TheirMsgPlace();
    their_msg_place = std::move(user_text_msg);

    ipc_resources_->TheirWriteSemaphore().Inc();
}

bool MessageIO::CanWriteImmediately() const {
    return ipc_resources_->TheirWriteSemaphore().CanDecImmediately();
}

std::string MessageIO::Read() {
    // TODO: Wake up sometimes to check control messages (or just increment semaphore on them?)
    // NOTE: For periodical wakeups see semtimedop
    // TODO: Statuses (here and everywhere)
    ipc_resources_->OurReadSemaphore().Dec();

    // TODO: Check control area

    auto const& payload = ipc_resources_->OurMsgPlace().payload;

    // TODO: Write logs when message does not contain teminator?
    // Currently I don't really want to deal with any logs
    std::string message{payload, std::ranges::find(payload, '\0')};
    return message;
}

bool MessageIO::CanReadImmediately() const {
    return ipc_resources_->OurReadSemaphore().CanDecImmediately();
}
