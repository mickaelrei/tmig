#pragma once

#include "tmig/physics/collision/collider.hpp"

namespace tmig {

namespace physics {

namespace collision {
    
/// @brief Collider specific for spheres
class SphereCollider : public Collider {
public:
    /// @brief Default constructor
    SphereCollider() = default;

    /// @brief Constructor with sphere data
    /// @param center read-only pointer to sphere center in world space
    /// @param radius read-only pointer to sphere radius
    SphereCollider(const glm::vec3 *center, const float *radius);

    /// @brief Gets the furthest point in a direction, based on the sphere center and radius
    /// @param direction direction to check
    /// @return furthest point in direction
    glm::vec3 furthestPoint(const glm::vec3 &direction) const override;

private:
    /// @brief Read-only pointer to sphere center in world space
    const glm::vec3 *center;

    /// @brief Read-only pointer to sphere radius
    const float *radius;
};

} // namespace collision

} // namespace physics

} // namespace tmig