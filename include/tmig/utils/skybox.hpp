#pragma once

#include "tmig/entity.hpp"
#include "tmig/gl/texture_cube.hpp"

namespace tmig {

namespace utils {

class Skybox : public Entity
{
public:
    Skybox() = default;
    Skybox(const gl::TextureCube &textureCube);

    void draw(const glm::mat4 &mat = glm::mat4{1.0f}) const override;

private:
    gl::TextureCube textureCube;
};

} // namespace utils

} // namespace tmig