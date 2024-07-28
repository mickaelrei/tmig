#pragma once

#include <memory>

#include "tmig/gl/shader.hpp"

namespace tmig {

namespace utils {

/// @brief Returns the default skybox shader
/// @return Default skybox shader
std::shared_ptr<gl::Shader> skyboxShader();

/// @brief Returns the default entity shader
/// @return Default entity shader
std::shared_ptr<gl::Shader> entityShader();

} // namespace utils

} // namespace tmig