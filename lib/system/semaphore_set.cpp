#include "lib/system/semaphore_set.h"

#include <cerrno>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <vector>

#include <sys/sem.h>

#include "lib/system/semaphore.h"
#include "lib/system/system_error.h"

using namespace messenger::system;

union SemctlArg {
    int val;
    semid_ds* buf;
    unsigned short* array;
};

SemaphoreSet::SemaphoreSet(std::initializer_list<unsigned short> initial_values) {
    if (initial_values.size() == 0 ||
        initial_values.size() > std::numeric_limits<unsigned short>::max()) {
        throw std::invalid_argument("invalid semaphore count");
    }
    count_ = initial_values.size();

    id_ = semget(IPC_PRIVATE, static_cast<int>(count_), IPC_CREAT | 0600);
    if (id_ == -1) {
        count_ = 0;
        throw SystemError{"semget"};
    }

    std::vector<unsigned short> values{initial_values};
    SemctlArg arg{};
    arg.array = values.data();
    if (semctl(id_, 0, SETALL, arg) == -1) {
        int err_code = errno;
        Reset();
        throw SystemError(err_code, "semctl(SETALL)");
    }
}

void SemaphoreSet::Reset() noexcept {
    if (count_ > 0) {
        semctl(id_, 0, IPC_RMID);
    }

    count_ = 0;
}

Semaphore SemaphoreSet::operator[](std::size_t idx) const {
    if (idx >= count_) {
        throw std::out_of_range("semaphore index");
    }

    return Semaphore{id_, static_cast<unsigned short>(idx)};
}

SemaphoreSetView::SemaphoreSetView(int id) : id_(id) {
    semid_ds info{};
    SemctlArg arg{};
    arg.buf = &info;

    if (::semctl(id, 0, IPC_STAT, arg) == -1) {
        throw SystemError{"semctl(IPC_STAT)"};
    }
    count_ = info.sem_nsems;
}

Semaphore SemaphoreSetView::operator[](std::size_t idx) const {
    if (idx >= count_) {
        throw std::out_of_range("semaphore index");
    }

    return Semaphore{id_, static_cast<unsigned short>(idx)};
}
