#pragma once

#include <memory>

#include "tmig/gl/shader.hpp"

namespace tmig {

namespace gl {

/// @brief Returns the default skybox shader
/// @return Default skybox shader
Shader skyboxShader();

/// @brief Returns the default entity shader
/// @return Default entity shader
Shader entityShader();

} // namespace gl

} // namespace tmig