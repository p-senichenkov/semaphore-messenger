#pragma once

#include <type_traits>

namespace messenger::util {
template <typename E>
    requires std::is_enum_v<E>
bool constexpr IsFlagSet(E value, E flag) noexcept {
    return static_cast<std::underlying_type_t<E>>(value) &
           static_cast<std::underlying_type_t<E>>(flag);
}
}  // namespace messenger::util
