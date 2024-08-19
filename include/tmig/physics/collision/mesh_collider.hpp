#pragma once

#include "tmig/physics/collision/collider.hpp"
#include "tmig/mesh.hpp"

namespace tmig {

namespace physics {

namespace collision {

/// @brief Collider for a mesh with a list of vertices
class MeshCollider : public Collider {
public:
    /// @brief Default constructor
    MeshCollider() = default;

    /// @brief Constructor with vertices
    /// @param vertices vertices list
    MeshCollider(const Mesh &mesh);

    /// @brief Gets the furthest point in a direction, based on the list of vertices
    /// @param direction direction to check
    /// @return furthest point in direction
    glm::vec3 furthestPoint(const glm::vec3 &direction) const override;

private:
    /// @brief List of vertices for the mesh
    std::vector<glm::vec3> vertices;
};

} // namespace collision

} // namespace physics

} // namespace tmig