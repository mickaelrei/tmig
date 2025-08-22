#pragma once

#include <string>

#include <glm/glm.hpp>

namespace tmig::render::window {

/// @brief Initializes the window used by the engine
void init(
    int width = 600,
    int height = 600,
    const std::string& title = "tmig"
);

/// @brief Gets the current time since the render module was initialized
/// @return Runtime in seconds
/// @note Depends on `render::init()`
double getRuntime();

/// @brief Returns whether the window should close
/// @note Depends on `render::init()`
bool shouldClose();

/// @brief Sets window `shouldClose` flag
/// @param close Whether window shoud close
/// @note Depends on `render::init()`
void setShouldClose(bool close);

/// @brief Swap window buffers, should be called after all rendering for current frame is done
/// @note Depends on `render::init()`
void swapBuffers();

/// @brief Polls new window events, such as keyboard and mouse
/// @note Depends on `render::init()`
void pollEvents();

/// @brief Sets new window size
/// @param size New window size in screen coordinates
/// @note Depends on `render::init()`
void setSize(const glm::ivec2& size);

/// @brief Returns the current window size in screen coordinates
/// @note Depends on `render::init()`
glm::ivec2 getSize();

/// @brief Sets a new title for the window
/// @param title New window title
/// @note Depends on `render::init()`
void setTitle(const std::string& title);

/// @brief Gets the current state of a key
/// @param key GLFW key code
/// @return GLFW key state enum (GLFW_PRESS or GLFW_RELEASE)
/// @note Depends on `render::init()`
int getKeyState(int key);

/// @brief Get the current state of a mouse button
/// @param button GLFW mouse button code
/// @return GLFW mouse button state (GLFW_PRESS or GLFW_RELEASE)
int getMouseButtonState(int button);

/// @brief Set cursor mode
/// @param mode GLFW cursor mode
void setCursorMode(int mode);

/// @brief Set cursor position in screen coordinates relative to top-left corner
void setCursorPos(const glm::vec2& pos);

/// @brief Get cursor position in screen coordinates relative to top-left corner
glm::vec2 getCursorPos();

} // namespace tmig::render::window