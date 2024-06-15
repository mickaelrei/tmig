#pragma once

#include <glm/glm.hpp>

namespace tmig {

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color = glm::vec4{1.0f};
};

} // namespace tmig