#pragma once

#include "tmig/render/texture2D.hpp"

namespace tmig::util {

/// @brief Utility for rendering a texture in a quad covering the whole screen
/// @note This won't bind any framebuffer, not even the default (window) one; bind before calling this
void renderScreenQuadTexture(const render::Texture2D& texture);

} // namespace tmig::util