#pragma once

#include <cstdio>
#include <string>
#include <iomanip>
#include <sstream>

namespace tmig::util {

/// @brief Category for logging
enum class LogCategory {
    ENGINE,
    OPENGL,
    SHADER
};

/// @brief Severity for logging
enum class LogSeverity {
    INFO,
    WARNING,
    ERROR
};

#ifdef DEBUG

#include "glad/glad.h"

static inline const char* categoryColor(LogCategory cat) {
    switch (cat) {
        case LogCategory::ENGINE: return "\033[0;93m"; // Yellow
        case LogCategory::OPENGL: return "\033[0;96m"; // Cyan
        case LogCategory::SHADER: return "\033[0;95m"; // Magenta
    }
    return "\033[0m";
}

static inline const char* severityColor(LogSeverity sev) {
    switch (sev) {
        case LogSeverity::INFO:    return "\033[0;90m"; // Grey
        case LogSeverity::WARNING: return "\033[0;93m"; // Yellow
        case LogSeverity::ERROR:   return "\033[0;91m"; // Red
    }
    return "\033[0m";
}

template <typename... Args>
inline void logMessage(LogCategory cat, LogSeverity sev, const char* format, Args&&... args) {
    // Convert enums to strings
    std::string catStr;
    switch (cat) {
        case LogCategory::ENGINE: catStr = "ENGINE"; break;
        case LogCategory::OPENGL: catStr = "OPENGL"; break;
        case LogCategory::SHADER: catStr = "SHADER"; break;
    }

    std::string sevStr;
    switch (sev) {
        case LogSeverity::INFO:    sevStr = "INFO"; break;
        case LogSeverity::WARNING: sevStr = "WARN"; break;
        case LogSeverity::ERROR:   sevStr = "ERR";  break;
    }

    // Fixed width: pad category to 7 chars, severity to 5 chars
    std::ostringstream oss;
    oss << categoryColor(cat) << std::left << std::setw(6) << catStr << "\033[0m";
    oss << " | ";
    oss << severityColor(sev) << std::left << std::setw(4) << sevStr << "\033[0m";

    std::string prefix = "[ " + oss.str() + " ] ";

    std::string newFormat = prefix + format;
    if constexpr (sizeof...(args) > 0) {
        std::printf(newFormat.c_str(), std::forward<Args>(args)...);
    } else {
        std::printf("%s", newFormat.c_str());
    }
}

/// @brief Checks OpenGL errors
/// @param file File name
/// @param line File line
static inline void _glCheckError(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        const char* errName = "UNKNOWN_ERROR";
        switch (errorCode) {
        case GL_INVALID_ENUM:                  errName = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 errName = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             errName = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                errName = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               errName = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 errName = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errName = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        logMessage(
            LogCategory::OPENGL, LogSeverity::ERROR,
            "%s at %s:%d\n", errName, file, line
        );
    }
}

/// @brief Macro for checking OpenGL errors
#define glCheckError() tmig::util::_glCheckError(__FILE__, __LINE__)

#else

template <typename... Args>
inline void logMessage(LogCategory, LogSeverity, const char*, Args&&...) {}

/// @brief Macro for checking OpenGL errors
#define glCheckError()

#endif // DEBUG

} // namespace tmig::util