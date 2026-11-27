#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace messenger::log {
enum class ELogLevel {
    kDebug = 0,
    kInfo = 1,
};

#if not defined(MSG_LOG_LEVEL)
#define MSG_LOG_LEVEL L_INFO
#endif

template <typename T>
concept Writeable = requires(T const& val, std::ostream& os) { os << val; };

template <ELogLevel Level>
class LogWriter {
private:
    std::ostringstream oss_;

    static std::string LogLevelToString(ELogLevel level) {
        switch (level) {
            case ELogLevel::kDebug:
                return "DEBUG";
            case ELogLevel::kInfo:
                return "INFO";
        }
    }

public:
    LogWriter() {
        if constexpr (static_cast<int>(Level) <= MSG_LOG_LEVEL) {
            oss_ << "[" << LogLevelToString(Level) << "] {PID " << getpid() << "} ";
        }
    }

    ~LogWriter() {
        if constexpr (static_cast<int>(Level) <= MSG_LOG_LEVEL) {
            std::cerr << oss_.str() << '\n';
        }
    }

    template <Writeable T>
    LogWriter& operator<<(T const& val) {
        if constexpr (static_cast<int>(Level) <= MSG_LOG_LEVEL) {
            oss_ << val;
        }
        return *this;
    }
};
}  // namespace messenger::log

#define LOG(LEVEL) \
    ::messenger::log::LogWriter<::messenger::log::ELogLevel::k##LEVEL> {}
