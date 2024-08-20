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
    /// @param modelMatrix read-only pointer to model matrix
    MeshCollider(const Mesh &mesh, const glm::mat4 *modelMatrix);

    glm::vec3 furthestPoint(const glm::vec3 &direction) const override;

    glm::vec3 getCenter() const override;

private:
    /// @brief List of vertices for the mesh
    std::vector<glm::vec3> vertices;

    /// @brief Read-only pointer to model matrix
    const glm::mat4 *modelMatrix;
};

} // namespace collision

} // namespace physics

} // namespace tmig