#include <bitset>

#include <GLFW/glfw3.h>

#include "tmig/render/window.hpp"
#include "tmig/core/input.hpp"

namespace tmig::core::input {

static std::bitset<GLFW_KEY_LAST + 1> _previousKeyStates;
static std::bitset<GLFW_KEY_LAST + 1> _currentKeyStates;
static std::bitset<8> _previousMouseButtonStates;
static std::bitset<8> _currentMouseButtonStates;

void update() {
    // Copy last frame's state to previous state
    _previousKeyStates = _currentKeyStates;
    _previousMouseButtonStates = _currentMouseButtonStates;

    // 2. Poll the current state for all keys and buttons.
    for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
        _currentKeyStates[key] = (render::window::getKeyState(key) == GLFW_PRESS);
    }
    for (int button = 0; button < 8; ++button) {
        _currentMouseButtonStates[button] = (render::window::getMouseButtonState(button) == GLFW_PRESS);
    }
    
    // 3. Process all pending window events for the next frame.
    render::window::pollEvents();
}

bool isKeyPressed(Key key) {
    int keyCode = static_cast<int>(key);
    return _currentKeyStates[keyCode] && !_previousKeyStates[keyCode];
}

bool isKeyReleased(Key key) {
    int keyCode = static_cast<int>(key);
    return !_currentKeyStates[keyCode] && _previousKeyStates[keyCode];
}

bool isKeyDown(Key key) {
    return _currentKeyStates[static_cast<int>(key)];
}

bool isMouseButtonPressed(MouseButton button) {
    int buttonCode = static_cast<int>(button);
    return _currentMouseButtonStates[buttonCode] && !_previousMouseButtonStates[buttonCode];
}

bool isMouseButtonReleased(MouseButton button) {
    int buttonCode = static_cast<int>(button);
    return !_currentMouseButtonStates[buttonCode] && _previousMouseButtonStates[buttonCode];
}

bool isMouseButtonDown(MouseButton button) {
    return _currentMouseButtonStates[static_cast<int>(button)];
}

glm::vec2 getCursorPos() {
    return render::window::getCursorPos();
}

void setCursorPos(const glm::vec2& pos) {
    render::window::setCursorPos(pos);
}

CursorMode getCursorMode() {
    return static_cast<CursorMode>(render::window::getCursorMode());
}

void setCursorMode(CursorMode mode) {
    render::window::setCursorMode(static_cast<int>(mode));
}

} // namespace tmig::core::input