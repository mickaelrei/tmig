#pragma once

#include <string>

namespace tmig::util {

#ifdef DEBUG

template <typename... Args>
void debugPrint(const char* format, Args&& ...args) {
    std::string newFormat = std::string{"\033[0;93mDEBUG:\033[0m "} + format;
    printf(newFormat.c_str(), std::forward<Args>(args)...);
}

/// @brief Checks OpenGL errors
/// @param file File name
/// @param line File line
void _glCheckError(const char* file, int line);

/// @brief Macro for checking OpenGL errors
#define glCheckError() tmig::util::_glCheckError(__FILE__, __LINE__)

#else

template <typename... Args>
void debugPrint(const char* format, [[maybe_unused]] Args&& ...args) {
    (void)format;
}

/// @brief Macro for checking OpenGL errors
#define glCheckError()

#endif // DEBUG

} // namespace tmig::util