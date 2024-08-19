#pragma once

#include "tmig/render/entity.hpp"
#include "tmig/render/gl/texture_cube.hpp"

namespace tmig {

namespace render {

namespace utils {

/// @brief Utility class for a scene Skybox
class Skybox : public Entity {
public:
    /// @brief Default constructor (does nothing)
    Skybox() = default;

    /// @brief Constructor
    /// @param textureCube texture cube to be used as skybox
    Skybox(const std::shared_ptr<gl::TextureCube> &textureCube);

    /// @brief Draw the skybox
    /// @param shader scene shader
    void draw(const gl::Shader& shader) const override;

private:
    /// @brief Skybox texture cube
    std::shared_ptr<gl::TextureCube> textureCube;
};

} // namespace utils

} // namespace render

} // namespace tmig