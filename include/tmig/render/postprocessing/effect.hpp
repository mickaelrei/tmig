#pragma once

#include <glm/glm.hpp>

#include "tmig/render/texture2D.hpp"
#include "tmig/core/non_copyable.hpp"
#include "tmig/render/camera.hpp"

namespace tmig::render::postprocessing {

/// @brief Contains all data available for a post-processing pass.
/// Effects can pick what they need from this struct.
struct PostProcessContext {
    /// @brief Interpreted as scene world position data
    const Texture2D* gPosition = nullptr;

    /// @brief Interpreted as scene normal data
    const Texture2D* gNormal = nullptr;

    /// @brief Scene projection matrix
    const glm::mat4* projection = nullptr;

    /// @brief Scene camera
    const Camera* camera = nullptr;
};

/// @brief Interface for a post-processing effect
class Effect : protected core::NonCopyable {
public:
    virtual ~Effect() = default;

    /// @brief Applies the effect on a provided input texture
    /// @param input Will be interpreted as the input scene render data
    /// @return A texture with the final effect rendered onto it
    virtual const Texture2D& apply(const Texture2D& input, const PostProcessContext& ctx = {}) = 0;
};

} // namespace tmig::render::postprocessing