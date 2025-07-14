#include <stdexcept>
#include <memory>

#include "glad/glad.h"

#include "tmig/render/render.hpp"
#include "tmig/render/window.hpp"

#include "tmig/util/debug.hpp"

// Flag for initialized
static bool initialized = false;

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
}

bool isInitialized() {
    return initialized;
}

void setClearColor(const glm::vec4 &color) {
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