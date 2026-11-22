#include "lib/core/system/semaphore_set.h"

#include <cerrno>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <vector>

#include <sys/sem.h>

#include "lib/core/system/semaphore.h"
#include "lib/core/system/system_error.h"

using namespace messenger::system;

namespace {
union SemctlArg {
    int val;
    semid_ds* buf;
    unsigned short* array;
};

std::vector<Semaphore> CreateSemaphores(int id, unsigned short count) {
    std::vector<Semaphore> result;
    result.reserve(count);
    for (unsigned short idx = 0; idx < count; ++idx) {
        result.emplace_back(id, idx);
    }
    return result;
}
}  // namespace

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

std::vector<Semaphore> SemaphoreSet::Semaphores() const {
    return CreateSemaphores(id_, count_);
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

std::vector<Semaphore> SemaphoreSetView::Semaphores() const {
    return CreateSemaphores(id_, count_);
}
