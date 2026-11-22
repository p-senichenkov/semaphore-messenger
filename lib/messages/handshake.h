#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace messenger {
// Handshake protocol:
// Slave gets Master PID (as a parameter)
//  Slave -> Master: REQ
// Master creates IPC resources
//  Master -> Slave: IPC IDs
// Slave connects to IPC resources
//  Slave -> Master: ACK
// If something goes wrong at any step, RST can be sent. On RST:
//  - Master deletes IPC resources
//  - Slave disconnects from IPC resources
//  - *No retries*. Everyone dies

// If not specified, payload is empty
enum class EControlMsgKind : uint8_t {
    // Connection request
    kReq,
    // Send IDs of IPC objects. Payload must be IpcIDs
    kIpcIDs,
    // Accept connection and start messaging
    kAck,
    // Connection reset
    kRst,
};

inline std::string ControlMsgKindToString(EControlMsgKind kind) {
    switch (kind) {
        case EControlMsgKind::kReq:
            return "Connection request";
        case EControlMsgKind::kIpcIDs:
            return "IPC IDs";
        case EControlMsgKind::kAck:
            return "Connection accept";
        case EControlMsgKind::kRst:
            return "Connection reset";
    }
}

struct IPCResourceIndices {
    unsigned char master_queue_idx;
    unsigned char slave_queue_idx;

    unsigned short master_read_sem_idx;
    unsigned short master_write_sem_idx;
    unsigned short slave_read_sem_idx;
    unsigned short slave_write_sem_idx;
};

struct IpcIDs {
    int shared_memory_id;
    int semaphore_set_id;

    IPCResourceIndices indices;
};

struct ControlMsg {
    EControlMsgKind kind;

    union ControlPayload {
        struct Empty {
        } empty [[no_unique_address]];

        IpcIDs ipc_ids;
    } payload;
};

inline ControlMsg KindOnly(EControlMsgKind kind) {
    return ControlMsg{.kind = kind, .payload = {.empty = {}}};
}
}  // namespace messenger
