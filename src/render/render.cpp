#include <stdexcept>
#include <memory>

#include "glad/glad.h"

#include "tmig/render/render.hpp"
#include "tmig/render/window.hpp"

#include "tmig/util/log.hpp"

// Flag for initialized
static bool initialized = false;

#ifdef DEBUG
static void debugMessageCallback(
    GLenum source,
    GLenum,
    GLuint,
    GLenum severity,
    GLsizei,
    const GLchar* message,
    const void*
) {
	using namespace tmig::util;

    // Filter out notifications
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    LogCategory category = LogCategory::OPENGL;
    if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
        category = LogCategory::SHADER;
    }

    LogSeverity sev = LogSeverity::INFO;
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM) {
        sev = LogSeverity::WARNING;
    } else if (severity == GL_DEBUG_SEVERITY_HIGH) {
        sev = LogSeverity::ERROR;
    }

    logMessage(category, sev, "%s\n", message);
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