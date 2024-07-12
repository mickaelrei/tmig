#pragma once

#include <vector>

#include "tmig/structs.hpp"

namespace tmig {

/// @brief Struct to represent a 3D mesh
struct Mesh {
    /// @brief Default constructor
    Mesh() = default;

    /// @brief Constructor
    /// @param vertices list of vertices
    /// @param indices list of indices
    Mesh(
        const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices
    );

    /// @brief List of vertices
    std::vector<Vertex> vertices;

    /// @brief List of indices
    std::vector<unsigned int> indices;
};

} // namespace tmig