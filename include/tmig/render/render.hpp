#pragma once

#include <glm/glm.hpp>

#include "tmig/render/camera.hpp"

namespace tmig::render {

/// @brief Initializes everything needed by render module
void init();

/// @brief Returns whether the render module was initialized
bool isInitialized();

/// @brief Set window background clear color
/// @param color New background RGBA color
/// @note Depends on `render::init()`
void setClearColor(const glm::vec4& color);

/// @brief Clear window buffers
/// @note Depends on `render::init()`
void clearBuffers(); // TODO: Add an option to choose which buffers to clear (color, depth, stencil etc)

/// @brief Sets the line width when rendering on wireframe mode
/// @param width Line width in pixels
/// @note Depends on `render::init()`
void setLineWidth(float width);

/// @brief Sets the line width when rendering on wireframe mode
/// @param width Line width in pixels
/// @note Depends on `render::init()`
void setPointSize(float size);

} // namespace tmig::render