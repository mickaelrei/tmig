#pragma once

#include <string>

namespace tmig {

#ifdef DEBUG

template <typename... Args>
void debug_print(const char *format, Args &&...args) {
    std::string new_format = std::string{"\033[0;93mDEBUG:\033[0m "} + format;
    printf(new_format.c_str(), std::forward<Args>(args)...);
}

#else

template <typename... Args>
void debug_print(const char *format, [[maybe_unused]] Args &&...args) {
    (void)format;
}

#endif // DEBUG

}