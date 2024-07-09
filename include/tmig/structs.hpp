#pragma once

#include <glm/glm.hpp>

namespace tmig {

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

} // namespace tmig