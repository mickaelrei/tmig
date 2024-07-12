#pragma once

#include "tmig/entity.hpp"
#include "tmig/gl/texture_cube.hpp"

namespace tmig {

namespace utils {

/// @brief Utility class for a scene Skybox
class Skybox : public Entity {
public:
    /// @brief Default constructor (does nothing)
    Skybox() = default;

    /// @brief Constructor
    /// @param textureCube texture cube to be used as skybox
    Skybox(const gl::TextureCube &textureCube);

    /// @brief Draw the skybox
    /// @param shader scene shader
    void draw(const gl::Shader& shader) const override;

private:
    /// @brief Skybox texture cube
    gl::TextureCube textureCube;
};

} // namespace utils

} // namespace tmig