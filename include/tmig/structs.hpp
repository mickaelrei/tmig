#pragma once

#include <glm/glm.hpp>

namespace tmig {

/// @brief Struct to represent a single Vertex
struct Vertex {
    /// @brief Vertex position
    glm::vec3 pos;

    /// @brief Vertex texture coordinate
    glm::vec2 uv;

    /// @brief Vertex direction normal
    glm::vec3 normal;
};

} // namespace tmig