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

/// @brief Struct to represent a 3D mesh
struct Mesh {
    /// @brief List of vertices
    std::vector<Vertex> vertices;

    /// @brief List of indices
    std::vector<unsigned int> indices;
};

} // namespace tmig