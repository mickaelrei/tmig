#pragma once

namespace tmig::core::input {

/// @brief Key codes for keyboard input
enum class Key {
    Unknown = -1,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus,
    Period,
    Slash,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Semicolon = 59,
    Equal = 61,
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket,
    Backslash,
    RightBracket,
    GraveAccent = 96,
    World1 = 161,
    World2,
    Escape = 256,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, 
    KeyPad0 = 320, KeyPad1, KeyPad2, KeyPad3, KeyPad4, KeyPad5, KeyPad6, KeyPad7, KeyPad8, KeyPad9,
    KeyPadDecimal,
    KeyPadDivide,
    KeyPadMultiply,
    KeyPadSubtract,
    KeyPadAdd,
    KeyPadEnter,
    KeyPadEqual,
    LeftShift = 340,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu,
};

/// @brief Button codes for mouse input
enum class MouseButton {
    B1, B2, B3, B4, B5, B6, B7, B8,
    Left = B1,
    Right = B2,
    Middle = B3,
};

/// @brief State and behavior of the cursor
enum class CursorMode {
    /// @brief Visible and behaves normally
    Normal = 0x00034001,

    /// @brief Hidden when over the window, but doesn't lock
    Hidden = 0x00034002,

    /// @brief Hidden and locked to the window, providing virtual, unlimited movement
    Disabled = 0x00034003,
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