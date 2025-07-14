#include <stdio.h>

#include "glad/glad.h"

#include "tmig/util/debug.hpp"

namespace tmig::util {

#ifdef DEBUG

void _glCheckError(const char *file, int line) {
    GLenum errorCode;
    std::string error;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        std::string fileBold = "\033[0;00;1m";
        fileBold += file;
        fileBold += "\033[0m";
        error = "\033[0;91m" + error + "\033[0m";
        debugPrint("OpenGL error | %s:%d: %s\n", fileBold.c_str(), line, error.c_str());
    }
}

#endif

} // namespace tmig::util