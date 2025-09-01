#pragma once

namespace tmig::core::input {

/// @brief Key codes for keyboard input
enum class Key {
    UNKNOWN = -1,
    SPACE = 32,
    APOSTROPHE = 39,
    COMMA = 44,
    MINUS,
    PERIOD,
    SLASH,
    NUM0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    SEMICOLON = 59,
    EQUAL = 61,
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LEFT_BRACKET,
    BACKSLASH,
    RIGHT_BRACKET,
    GRAVE_ACCENT = 96,
    WORLD1 = 161,
    WORLD2,
    ESCAPE = 256,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DELETE,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
    KEYPAD0 = 320, KEYPAD1, KEYPAD2, KEYPAD3, KEYPAD4, KEYPAD5, KEYPAD6, KEYPAD7, KEYPAD8, KEYPAD9,
    KEYPAD_DECIMAL,
    KEYPAD_DIVIDE,
    KEYPAD_MULTIPLY,
    KEYPAD_SUBTRACT,
    KEYPAD_ADD,
    KEYPAD_ENTER,
    KEYPAD_EQUAL,
    LEFT_SHIFT = 340,
    LEFT_CONTROL,
    LEFT_ALT,
    LEFT_SUPER,
    RIGHT_SHIFT,
    RIGHT_CONTROL,
    RIGHT_ALT,
    RIGHT_SUPER,
    MENU,
};

/// @brief Button codes for mouse input
enum class MouseButton {
    B1, B2, B3, B4, B5, B6, B7, B8,
    LEFT = B1,
    RIGHT = B2,
    MIDDLE = B3,
};

/// @brief State and behavior of the cursor
enum class CursorMode {
    /// @brief Visible and behaves normally
    NORMAL = 0x00034001,

    /// @brief Hidden when over the window, but doesn't lock
    HIDDEN = 0x00034002,

    /// @brief Hidden and locked to the window, providing virtual, unlimited movement
    DISABLED = 0x00034003,
};

/// @brief Updates the input state. Should be called once at the start of each frame
void update();

/// @brief Checks if a key was pressed in the current frame
/// @return True if the key was pressed in this frame, false otherwise
bool isKeyPressed(Key key);

/// @brief Checks if a key was released in the current frame
/// @return True if the key was released in this frame, false otherwise
bool isKeyReleased(Key key);

/// @brief Checks if a key is currently being held down
/// @return True if the key is held down, false otherwise
bool isKeyDown(Key key);

/// @brief Checks if a mouse button was pressed in the current frame
/// @return True if the mouse button was pressed in this frame, false otherwise
bool isMouseButtonPressed(MouseButton button);

/// @brief Checks if a mouse button was released in the current frame
/// @return True if the mouse button was released in this frame, false otherwise
bool isMouseButtonReleased(MouseButton button);

/// @brief Checks if a mouse button is currently being held down
/// @return True if the mouse button is held down, false otherwise
bool isMouseButtonDown(MouseButton button);

/// @brief Get current cursor position in pixel coordinates
glm::vec2 getCursorPos();

/// @brief Set cursor position in screen coordinates relative to top-left corner
void setCursorPos(const glm::vec2& pos);

/// @brief Get current cursor mode
CursorMode getCursorMode();

/// @brief Set cursor mode
void setCursorMode(CursorMode mode);

} // namespace tmig::core::input