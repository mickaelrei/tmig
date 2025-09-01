#pragma once

struct GLFWwindow;

namespace tmig::render::window {

/// @brief Returns the internal GLFWwindow handle for internal engine use
/// @note This function is not supposed to be used directly
GLFWwindow* getGlfwWindow();

} // namespace tmig::render::window
