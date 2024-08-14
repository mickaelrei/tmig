#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "tmig/camera.hpp"
#include "tmig/scene.hpp"

namespace tmig {

/// @brief GLFWWindow wrapper class
class Window {
public:
    /// @brief Wrapper enum for GLFW key codes
    enum class KeyCode {
        space = GLFW_KEY_SPACE,
        apostrophe = GLFW_KEY_APOSTROPHE,
        comma = GLFW_KEY_COMMA,
        minus = GLFW_KEY_MINUS,
        period = GLFW_KEY_PERIOD,
        slash = GLFW_KEY_SLASH,
        num0 = GLFW_KEY_0,
        num1 = GLFW_KEY_1,
        num2 = GLFW_KEY_2,
        num3 = GLFW_KEY_3,
        num4 = GLFW_KEY_4,
        num5 = GLFW_KEY_5,
        num6 = GLFW_KEY_6,
        num7 = GLFW_KEY_7,
        num8 = GLFW_KEY_8,
        num9 = GLFW_KEY_9,
        semicolon = GLFW_KEY_SEMICOLON,
        equal = GLFW_KEY_EQUAL,
        a = GLFW_KEY_A,
        b = GLFW_KEY_B,
        c = GLFW_KEY_C,
        d = GLFW_KEY_D,
        e = GLFW_KEY_E,
        f = GLFW_KEY_F,
        g = GLFW_KEY_G,
        h = GLFW_KEY_H,
        i = GLFW_KEY_I,
        j = GLFW_KEY_J,
        k = GLFW_KEY_K,
        l = GLFW_KEY_L,
        m = GLFW_KEY_M,
        n = GLFW_KEY_N,
        o = GLFW_KEY_O,
        p = GLFW_KEY_P,
        q = GLFW_KEY_Q,
        r = GLFW_KEY_R,
        s = GLFW_KEY_S,
        t = GLFW_KEY_T,
        u = GLFW_KEY_U,
        v = GLFW_KEY_V,
        w = GLFW_KEY_W,
        x = GLFW_KEY_X,
        y = GLFW_KEY_Y,
        z = GLFW_KEY_Z,
        leftBracket = GLFW_KEY_LEFT_BRACKET,
        backslash = GLFW_KEY_BACKSLASH,
        rightBracket = GLFW_KEY_RIGHT_BRACKET,
        graveAccent = GLFW_KEY_GRAVE_ACCENT,
        world1 = GLFW_KEY_WORLD_1,
        world2 = GLFW_KEY_WORLD_2,
        escape = GLFW_KEY_ESCAPE,
        enter = GLFW_KEY_ENTER,
        tab = GLFW_KEY_TAB,
        backspace = GLFW_KEY_BACKSPACE,
        insert = GLFW_KEY_INSERT,
        keyDelete = GLFW_KEY_DELETE,
        right = GLFW_KEY_RIGHT,
        left = GLFW_KEY_LEFT,
        down = GLFW_KEY_DOWN,
        up = GLFW_KEY_UP,
        pageUp = GLFW_KEY_PAGE_UP,
        pageDown = GLFW_KEY_PAGE_DOWN,
        home = GLFW_KEY_HOME,
        end = GLFW_KEY_END,
        capsLock = GLFW_KEY_CAPS_LOCK,
        scrollLock = GLFW_KEY_SCROLL_LOCK,
        numLock = GLFW_KEY_NUM_LOCK,
        printScreen = GLFW_KEY_PRINT_SCREEN,
        pause = GLFW_KEY_PAUSE,
        f1 = GLFW_KEY_F1,
        f2 = GLFW_KEY_F2,
        f3 = GLFW_KEY_F3,
        f4 = GLFW_KEY_F4,
        f5 = GLFW_KEY_F5,
        f6 = GLFW_KEY_F6,
        f7 = GLFW_KEY_F7,
        f8 = GLFW_KEY_F8,
        f9 = GLFW_KEY_F9,
        f10 = GLFW_KEY_F10,
        f11 = GLFW_KEY_F11,
        f12 = GLFW_KEY_F12,
        f13 = GLFW_KEY_F13,
        f14 = GLFW_KEY_F14,
        f15 = GLFW_KEY_F15,
        f16 = GLFW_KEY_F16,
        f17 = GLFW_KEY_F17,
        f18 = GLFW_KEY_F18,
        f19 = GLFW_KEY_F19,
        f20 = GLFW_KEY_F20,
        f21 = GLFW_KEY_F21,
        f22 = GLFW_KEY_F22,
        f23 = GLFW_KEY_F23,
        f24 = GLFW_KEY_F24,
        f25 = GLFW_KEY_F25,
        keyPad0 = GLFW_KEY_KP_0,
        keyPad1 = GLFW_KEY_KP_1,
        keyPad2 = GLFW_KEY_KP_2,
        keyPad3 = GLFW_KEY_KP_3,
        keyPad4 = GLFW_KEY_KP_4,
        keyPad5 = GLFW_KEY_KP_5,
        keyPad6 = GLFW_KEY_KP_6,
        keyPad7 = GLFW_KEY_KP_7,
        keyPad8 = GLFW_KEY_KP_8,
        keyPad9 = GLFW_KEY_KP_9,
        keyPadDecimal = GLFW_KEY_KP_DECIMAL,
        keyPadDivide = GLFW_KEY_KP_DIVIDE,
        keyPadMultiply = GLFW_KEY_KP_MULTIPLY,
        keyPadSubtract = GLFW_KEY_KP_SUBTRACT,
        keyPadAdd = GLFW_KEY_KP_ADD,
        keyPadEnter = GLFW_KEY_KP_ENTER,
        keyPadEqual = GLFW_KEY_KP_EQUAL,
        leftShift = GLFW_KEY_LEFT_SHIFT,
        leftControl = GLFW_KEY_LEFT_CONTROL,
        leftAlt = GLFW_KEY_LEFT_ALT,
        leftSuper = GLFW_KEY_LEFT_SUPER,
        rightShift = GLFW_KEY_RIGHT_SHIFT,
        rightControl = GLFW_KEY_RIGHT_CONTROL,
        rightAlt = GLFW_KEY_RIGHT_ALT,
        rightSuper = GLFW_KEY_RIGHT_SUPER,
        menu = GLFW_KEY_MENU,
        last = GLFW_KEY_LAST,
    };

    /// @brief Key state
    enum class KeyState {
        /// @brief Key is released
        released = GLFW_RELEASE,

        /// @brief Key is pressed
        pressed = GLFW_PRESS,

        /// @brief Key is repeatedly pressed (unused, works only on the key callback)
        repeat = GLFW_REPEAT,
    };

    /// @brief Default constructor
    Window();

    /// @brief Constructor
    /// @param title window title
    Window(const std::string &title);

    /// @brief Destructor
    ~Window();

    /// @brief Start rendering loop
    void start();

    /// @brief Set window title
    /// @param title new title
    void setTitle(const std::string &title);

    /// @brief Set window size
    /// @param size size vector
    void setSize(const glm::ivec2 &size) const;

    /// @brief Get window size
    /// @return Window size vector
    glm::ivec2 getSize() const;

    /// @brief Get cursor position
    /// @return Cursor position vector
    glm::vec2 getCursorPos() const;

    /// @brief Get elapsed time since engine initialization
    /// @return Elapsed time in seconds
    float elapsedTime() const;

    /// @brief Sets if window should close
    /// @param shouldClose should close flag
    void setShouldClose(bool shouldClose) const;

    /// @brief Get state from key
    /// @param key key code
    /// @return current state for key
    KeyState getKeyState(KeyCode key) const;

    /// @brief Check if key was pressed in the current frame
    /// @param key key to test
    /// @return Whether key was pressed in the called frame
    bool isKeyPressed(KeyCode key);

    /// @brief Get if key is being held
    /// @param key key to test
    /// @return Whether key is held in the called frame
    bool isKeyHeld(KeyCode key);

    /// @brief Get if key was released in the current frame
    /// @param key key to test
    /// @return Whether key was released in the called frame
    bool isKeyReleased(KeyCode key);

    /// @brief Setup objects, called once before render loop
    virtual void setup();

    /// @brief Update application, called on each render update
    /// @param dt delta time
    virtual void update(float dt);

protected:
    /// @brief Process input, called before each render update
    /// @param dt delta time
    virtual void processInput(float dt);

    /// @brief Sets state for given key
    /// @param key key to set
    /// @param state new key state
    void setKeyState(KeyCode key, KeyState state);

    /// @brief Updates previous keyboard state
    void updatePreviousKeyboardState();

    /// @brief Current window title
    std::string title;

    /// @brief Current render scene
    std::shared_ptr<Scene> currentScene = nullptr;

    /// @brief Projection matrix
    glm::mat4 projection;

    /// @brief Map containing the state of each key
    std::unordered_map<KeyCode, KeyState> keyboardState;

    /// @brief Map containing previous state of each key
    std::unordered_map<KeyCode, KeyState> keyboardPreviousState;

    /// @brief GLFW window object
    GLFWwindow *window;
};

} // namespace tmig