#include <memory>
#include <stdexcept>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "tmig/render/window.hpp"
#include "tmig/core/callback_manager.hpp"
#include "tmig/util/debug.hpp"

// Default callback for framebuffer resize
void defaultFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height); glCheckError();
    tmig::core::notifyWindowResize(width, height);
}

// Flag for initialized
static bool initialized = false;

// Local variable for the window used in the engine
static std::shared_ptr<GLFWwindow> glfwWindow = nullptr;

// Struct for terminating GLFW resources when window shared pointer gets deleted
struct WindowDeleter {
    void operator()(GLFWwindow* window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        initialized = false;
    }
};

namespace tmig::render::window {

void init(int width,int height, const std::string &title) {
    if (initialized) return;
    initialized = true;

    // Init GLFW
    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error{"Failed to initialize GLFW\n"};
    }

    // Set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA

    // Create window
    glfwWindow = std::shared_ptr<GLFWwindow>{
        glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr),
        WindowDeleter{}
    };

    // Set window callbacks
    glfwSetFramebufferSizeCallback(glfwWindow.get(), defaultFramebufferSizeCallback);

    // Load GLAD
    glfwMakeContextCurrent(glfwWindow.get());
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error{"Failed to initialize GLAD\n"};
    }
}

double getRuntime() {
    return glfwGetTime();
}

bool shouldClose() {
#ifdef DEBUG
    if (!initialized) return false;
#endif

    return static_cast<bool>(glfwWindowShouldClose(glfwWindow.get()));
}

void setShouldClose(bool close) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSetWindowShouldClose(glfwWindow.get(), static_cast<int>(close));
}

void swapBuffers() {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSwapBuffers(glfwWindow.get());
}

void pollEvents() {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwPollEvents();
}

void setSize(const glm::ivec2& size) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSetWindowSize(glfwWindow.get(), size.x, size.y);
}

glm::ivec2 getSize() {
    glm::ivec2 size{0, 0};

#ifdef DEBUG
    if (!initialized) return size;
#endif

    glfwGetWindowSize(glfwWindow.get(), &size.x, &size.y);
    return size;
}

void setTitle(const std::string& title) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSetWindowTitle(glfwWindow.get(), title.c_str());
}

int getKeyState(int key) {
#ifdef DEBUG
    if (!initialized) return -1;
#endif

    return glfwGetKey(glfwWindow.get(), key);
}

int getMouseButtonState(int button) {
#ifdef DEBUG
    if (!initialized) return -1;
#endif

    return glfwGetMouseButton(glfwWindow.get(), button);
}

void setCursorMode(int mode) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSetInputMode(glfwWindow.get(), GLFW_CURSOR, mode);
}

void setCursorPos(const glm::vec2& pos) {
#ifdef DEBUG
    if (!initialized) return;
#endif

    glfwSetCursorPos(glfwWindow.get(), pos.x, pos.y);
}

glm::vec2 getCursorPos() {
    double x, y;

#ifdef DEBUG
    if (!initialized) return glm::vec2{};
#endif

    glfwGetCursorPos(glfwWindow.get(), &x, &y);
    return glm::vec2{x, y};
}

} // namespace tmig::render::window