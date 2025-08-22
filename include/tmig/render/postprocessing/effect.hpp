#pragma once

#include "tmig/render/texture2D.hpp"
#include "tmig/core/non_copyable.hpp"

namespace tmig::render::postprocessing {

/// @brief Interface for a post-processing effect
class Effect : protected core::NonCopyable {
public:
    /// @brief Applies the effect on a provided input texture
    /// @param input Will be interpreted as the input scene render data
    /// @return A texture with the final effect rendered onto it
    virtual const Texture2D& apply(const Texture2D& input) = 0;
};

} // namespace tmig::render::postprocessing