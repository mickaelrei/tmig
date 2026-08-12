#pragma once

#include "tmig/render/texture2D.hpp"

namespace tmig::util {

/// @brief Utility for rendering a texture in a quad covering the whole screen
/// @note This won't bind any framebuffer, not even the default (window) one; bind before calling this
void renderScreenQuadTexture(const render::Texture2D& texture);

/// @brief Renders `left` on the left half of the screen and `right` on the right half.
/// Both halves share the same UVs, so aspect ratio and camera projection stay correct.
/// @note Bind the target framebuffer (usually the default one) before calling this
void renderScreenQuadSplit(const render::Texture2D& left, const render::Texture2D& right);

} // namespace tmig::util