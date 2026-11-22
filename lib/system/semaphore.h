#pragma once

#include <cerrno>

#include <sys/sem.h>

#include "lib/system/system_error.h"

namespace messenger::system {
/// @brief Non-owning semaphore handle
class Semaphore {
public:
    Semaphore(int set_id, unsigned short index) noexcept : set_id_(set_id), index_(index) {}

    void Dec() const {
        Operate(-1);
    }

    void Inc() const {
        Operate(+1);
    }

    bool CanDecImmediately() const {
        return Get() > 0;
    }

private:
    void Operate(short delta) const {
        sembuf op{index_, delta, 0};
        while (semop(set_id_, &op, 1) == -1) {
            if (errno == EINTR) {
                continue;
            }
            throw SystemError{"semop"};
        }
    }

    int Get() const {
        int val = semctl(set_id_, index_, GETVAL);
        if (val < 0) {
            throw SystemError{"semctl"};
        }
        return val;
    }

    int set_id_;
    unsigned short index_;
};
}  // namespace messenger::system
