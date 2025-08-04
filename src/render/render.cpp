#include <stdexcept>
#include <memory>

#include "glad/glad.h"

#include "tmig/render/render.hpp"
#include "tmig/render/window.hpp"

#include "tmig/util/debug.hpp"

// Flag for initialized
static bool initialized = false;

#ifdef DEBUG
static void debugMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    (void)length;
    (void)userParam;

	auto const src_str = [source]() {
		switch (source) {
		case GL_DEBUG_SOURCE_API:             return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY:     return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION:     return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER:           return "OTHER";
		}
        return "UNKNOWN";
	}();

	auto const type_str = [type]() {
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:               return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY:         return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE:         return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER:              return "MARKER";
		case GL_DEBUG_TYPE_OTHER:               return "OTHER";
		}
        return "UNKNOWN";
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW:          return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM:       return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH:         return "HIGH";
		}
        return "UNKNOWN";
	}();

    printf("\033[0;93m[(%d) %s | %s - %s]\033[0m %s\n", id, src_str, type_str, severity_str, message);
}
#endif

namespace tmig::render {

void init() {
    if (initialized) return;
    initialized = true;

    // Initialize window module
    window::init();

    // Set some default OpenGL flags
    glEnable(GL_DEPTH_TEST); glCheckError();
    glEnable(GL_CULL_FACE); glCheckError();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glCheckError();
    glEnable(GL_BLEND); glCheckError();
    //glfwSwapInterval(0); glCheckError(); // Disable VSync
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugMessageCallback, nullptr);
#endif
}

bool isInitialized() {
    return initialized;
}

void setClearColor(const glm::vec4& color) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glClearColor(color.x, color.y, color.z, color.w); glCheckError();
}

void clearBuffers() {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glCheckError();
}

void setLineWidth(float width) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glLineWidth(width); glCheckError();
}

void setPointSize(float size) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glPointSize(size); glCheckError();
}

} // namespace tmig::render