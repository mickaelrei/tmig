#include <iostream>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include "glad/glad.h"

#include "tmig/init.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/utils/shaders.hpp"
#include "tmig/render/gl/shader.hpp"
#include "tmig/render/entity.hpp"
#include "tmig/render/utils/skybox.hpp"

#include "tmig/render/utils/primitives_gmesh.hpp"

namespace tmig {

namespace render {

// Window frame buffer resize callback
static void default_frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (w == nullptr) return;

    w->frameBufferSizeCallback(width, height);
}

// Window mouse scroll callback
static void default_scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (w == nullptr) return;

    w->scrollCallback((float)xOffset, (float)yOffset);
}

Window::Window()
    : Window::Window("Tmig Engine") {}

Window::Window(const std::string &title)
    : title{title}
{
    // Create window
    window = glfwCreateWindow(600, 600, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        throw std::runtime_error{"Failed to create GLFW window\n"};
        return;
    }

    // Set callbacks
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, default_frameBufferSizeCallback);
    glfwSetScrollCallback(window, default_scrollCallback);

    // Initialize GLAD
    initGLAD();
}

Window::~Window()
{
    glfwDestroyWindow(window);
}

void Window::setup() {}

void Window::update(float dt)
{
    (void)dt;
}

void Window::start() {
    // Call setup before starting
    setup();

    float last = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        // Clear screen
        glm::vec4 clearColor{0.05f, 0.05f, 0.05f, 1.0f};
        if (currentScene != nullptr) {
            clearColor = currentScene->clearColor;
        }
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate delta time
        float t = glfwGetTime();
        float dt = t - last;
        last = t;

        // Call process input
        processInput(dt);

        if (currentScene != nullptr) {
            utils::entityShader()->setVec3("viewPos", currentScene->camera.pos);
        }

        // Call update
        update(dt);

        // Update keyboard state
        updatePreviousKeyboardState();

        // Update GLFW resources
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Window::setTitle(const std::string &title)
{
    Window::title = title;
    glfwSetWindowTitle(window, title.c_str());
}

void Window::setSize(const glm::ivec2 &size) const
{
    glfwSetWindowSize(window, size.x, size.y);
}

glm::ivec2 Window::getSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(window, &size.x, &size.y);
    return size;
}

glm::vec2 Window::getCursorPos() const {
    glm::dvec2 pos;
    glfwGetCursorPos(window, &pos.x, &pos.y);
    return glm::vec2{pos};
}

void Window::setCursorPos(const glm::vec2 &pos) const {
    glfwSetCursorPos(window, (double)pos.x, (double)pos.y);
}

float Window::elapsedTime() const {
    return (float)glfwGetTime();
}

void Window::processInput(float dt)
{
    // Check for window close
    if (isKeyPressed(KeyCode::escape))
    {
        setShouldClose(true);
    }

    if (currentScene == nullptr) return;

    auto &cam = currentScene->camera;

    // Camera movement
    if (isKeyHeld(KeyCode::w))
    {
        cam.moveForward(dt);
    }
    if (isKeyHeld(KeyCode::s))
    {
        cam.moveBack(dt);
    }
    if (isKeyHeld(KeyCode::d))
    {
        cam.moveRight(dt);
    }
    if (isKeyHeld(KeyCode::a))
    {
        cam.moveLeft(dt);
    }
    if (isKeyHeld(KeyCode::e))
    {
        cam.moveUp(dt);
    }
    if (isKeyHeld(KeyCode::q))
    {
        cam.moveDown(dt);
    }

    if (isMouseKeyPressed(MouseKey::right)) {
        setCursorMode(CursorMode::disabled);

        // Get screen center
        auto screenCenter = glm::vec2{getSize()} * 0.5f;

        // If first click, reset to center
        if (currentScene->camera.firstClick) {
            setCursorPos(screenCenter);
        }

        currentScene->camera.firstClick = false;

        // Get offset from center and rotate
        auto pos = getCursorPos();
        auto offset = pos - screenCenter;
        currentScene->camera.rotate(-offset.y * dt, -offset.x * dt);

        // Set cursor back to center
        setCursorPos(screenCenter);
    } else {
        // Show cursor again
        setCursorMode(CursorMode::normal);
        currentScene->camera.firstClick = true;
    }
}

void Window::setShouldClose(bool shouldClose) const {
    glfwSetWindowShouldClose(window, shouldClose);
}

Window::KeyState Window::getKeyState(KeyCode key) const {
    auto keyInt = static_cast<int>(key);
    return static_cast<KeyState>(glfwGetKey(window, keyInt));
}

void Window::setKeyState(KeyCode key, KeyState state) {
    keyboardState[key] = state;
}

void Window::updatePreviousKeyboardState() {
    for (auto pair : keyboardState) {
        keyboardPreviousState[pair.first] = pair.second;
    }
}

bool Window::isKeyPressed(KeyCode key) {
    auto newState = getKeyState(key);
    auto previousState = keyboardPreviousState[key];
    setKeyState(key, newState);

    // Key is pressed if it was released and got pressed now
    return newState == KeyState::pressed && previousState == KeyState::released;
}

bool Window::isKeyHeld(KeyCode key) {
    auto newState = getKeyState(key);
    setKeyState(key, newState);

    // Key is held if it is pressed
    return newState == KeyState::pressed;
}

bool Window::isKeyReleased(KeyCode key) {
    auto newState = getKeyState(key);
    auto previousState = keyboardPreviousState[key];
    setKeyState(key, newState);

    // Key is released if it was pressed and got released now
    return newState == KeyState::released && previousState == KeyState::pressed;
}

Window::MouseKeyState Window::getKeyState(MouseKey key) const {
    int keyInt = static_cast<int>(key);
    return static_cast<MouseKeyState>(glfwGetMouseButton(window, keyInt));
}

bool Window::isMouseKeyPressed(MouseKey key) const {
    return getKeyState(key) == MouseKeyState::pressed;
}

void Window::setCursorMode(CursorMode mode) const {
    int modeInt = static_cast<int>(mode);
    glfwSetInputMode(window, GLFW_CURSOR, modeInt);
}

void Window::frameBufferSizeCallback(int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::scrollCallback(float xOffset, float yOffset) {
    (void)xOffset;
    (void)yOffset;
}

} // namespace render

} // namespace tmig