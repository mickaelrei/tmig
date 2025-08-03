#pragma once

#include <functional>
#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

namespace tmig::util {

/// @brief General vertex data generated from utility functions
struct GeneralVertex {
    /// @brief Vertex position in global coordinates
    glm::vec3 position;

    /// @brief Vertex normal direction
    glm::vec3 normal;

    /// @brief Texture coordinates
    glm::vec2 uv;
};

/// @brief Type definition for a callback for generated vertices
typedef std::function<void(const GeneralVertex& vertex)> VertexGenerateCallback;


/// @brief Generates a sphere mesh by passing vertices to callback and filling indices vector
/// @param resolution Number of vertex rows/columns (mesh quality)
void generateSphereMesh(
    const VertexGenerateCallback& vertexCallback,
    std::vector<uint32_t>& indices,
    const uint32_t resolution = 50
);

/// @brief Generates a cylinder mesh by passing vertices to callback and filling indices vector
/// @param resolution Number of vertex rows/columns (mesh quality)
void generateCylinderMesh(
    const VertexGenerateCallback& vertexCallback,
    std::vector<uint32_t>& indices,
    const uint32_t resolution = 20
);

/// @brief Generates a cone mesh by passing vertices to callback and filling indices vector
/// @param resolution Number of vertex rows/columns (mesh quality)
void generateConeMesh(
    const VertexGenerateCallback& vertexCallback,
    std::vector<uint32_t>& indices,
    const uint32_t resolution = 20
);

/// @brief Generates a torus mesh by passing vertices to callback and filling indices vector
/// @param resolution Number of vertex rows/columns (mesh quality)
void generateTorusMesh(
    const VertexGenerateCallback& vertexCallback,
    std::vector<uint32_t>& indices,
    const uint32_t resolution = 50
);

/// @brief Generates a box mesh by passing vertices to callback and filling indices vector
void generateBoxMesh(const VertexGenerateCallback& vertexCallback, std::vector<uint32_t>& indices);

/// @brief Generates a wedge mesh by passing vertices to callback and filling indices vector
void generateWedgeMesh(const VertexGenerateCallback& vertexCallback, std::vector<uint32_t>& indices);

} // namespace tmig::util