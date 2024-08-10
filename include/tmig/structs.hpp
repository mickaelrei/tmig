#pragma once

#include <glm/glm.hpp>

namespace tmig {

/// @brief Struct to represent a single Vertex
struct Vertex {
    /// @brief Vertex position
    glm::vec3 pos = glm::vec3{0.0f};

    /// @brief Vertex texture coordinate
    glm::vec2 uv = glm::vec2{0.0f};

    /// @brief Vertex direction normal
    glm::vec3 normal = glm::vec3{0.0f, 0.0f, 1.0f};
};

} // namespace tmig